#include "cdaemon.h"

CServer::CServer(io_service& io_service)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {

	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	CCommandFactory::add<CCmdGetFile>(ECommand::GET_FILE);
	CCommandFactory::add<CCmdGetMD5>(ECommand::GET_MD5);
	CCommandFactory::add<CCmdUploadFile>(ECommand::UPLOAD_FILE);

	_start_accept();	
}

void CServer::_start_accept() {
	BOOST_LOG_TRIVIAL(info) << "Waiting for client...";

	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.get_io_service());
	m_acceptor.async_accept(connection->m_socket, boost::bind(&CServer::_handle_accept, this, connection, 
				placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	BOOST_LOG_TRIVIAL(info) << "Accepting client...";
	if (!ec) {
		BOOST_LOG_TRIVIAL(info) << "New client accepted; reading command...";
		async_read_until(connection->m_socket, connection->m_readbuf, "\n",
				 boost::bind(&CTCPConnection::handle_read_command, connection,
				 placeholders::error));
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "Unable to accept client: " + ec.message();
	}
	_start_accept();
}

void CTCPConnection::_send_feedback(EError error) {
	BOOST_LOG_TRIVIAL(info) << "Sending feedback...";
	std::vector<char> payload({ static_cast<char>(error) });
	CMessage feedback(ECommand::FEEDBACK, EDataType::ERROR_CODE, payload);
	feedback.to_streambuf(m_writebuf);
	async_write(m_socket, m_writebuf,
		    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
		    placeholders::error));
}

void CTCPConnection::handle_recv_message(const boost::system::error_code& ec) {
	if (!ec || ec == error::eof) {
		BOOST_LOG_TRIVIAL(info) << "Processing incoming message...";
		boost::shared_ptr<CMessage> msg(new CMessage);
		EError ret = msg->from_streambuf(m_readbuf);
		m_readbuf.consume(m_readbuf.size());
		if (ret != EError::OK) {
			BOOST_LOG_TRIVIAL(warning) << get_text_error(ret);
			_send_feedback(ret);
			return;
		}
		_process_message(msg);
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "handle_recv_message: " + ec.message();
	}
}

void CTCPConnection::_process_message(boost::shared_ptr<CMessage> msg) {
	auto cmd = CCommandFactory::create(*msg);
	if (cmd == NULL) {
		_send_feedback(EError::UNKNOWN_COMMAND);
		return;
	}
	cmd->extract_args_from_message(*msg);
	EError ret = cmd->invoke(m_context, msg->datatype());
	if (ret != EError::OK) {
		m_context->async_send_feedback(ret);
	}
	if (cmd) {
		delete cmd;
	}
}

void CTCPConnection::handle_write_response(const boost::system::error_code& ec) {
	m_writebuf.consume(m_writebuf.size());
	if (!ec) { 
		BOOST_LOG_TRIVIAL(info) << "Response transmitted successfully";
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "handle_write_response: " +  ec.message();
	}
}

CTCPConnection::conn_ptr CTCPConnection::create(io_service& io_service) {
	return conn_ptr(new CTCPConnection(io_service));
}

CDaemon::CDaemon(const CParser& parser) : m_io_service(new io_service()) {
	CSettings::set_working_dir("/var/frtp/");

	logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
	logging::add_file_log(
			keywords::file_name = parser.logname(),
			keywords::auto_flush = true,
			keywords::open_mode = (std::ios::out | std::ios::app),
			keywords::format = "%TimeStamp% [%Uptime%] (%LineID%) <%Severity%>: %Message%"
			);
	logging::add_common_attributes();
	logging::core::get()->add_global_attribute("Uptime", attrs::timer());
}

int CDaemon::start() {
	signal_set signals(*(m_io_service.get()), SIGINT, SIGTERM);
	signals.async_wait(boost::bind(&io_service::stop, m_io_service));
	m_io_service->notify_fork(io_service::fork_prepare);

	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "CDaemon::start(): first fork failed.";
			return 1;
		}
	}

	setsid();
	chdir("/");
	umask(0);
	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "CDaemon::start(): second fork failed.";
			return 1;
		}
	}
	close(0);
	close(1);
	close(2);

	if (open("/dev/null", O_RDONLY) < 0) {
		BOOST_LOG_TRIVIAL(error) << "CDaemon::start(): unable to open /dev/null.";
		return 1;
	}

	m_io_service->notify_fork(io_service::fork_child);

	BOOST_LOG_TRIVIAL(info) << "CDaemon::start(): daemon started.";
	CServer server(*(m_io_service.get()));
	m_io_service->run();
	BOOST_LOG_TRIVIAL(info) << "CDaemon::start(): daemon stopped.";

	return 0;
}
