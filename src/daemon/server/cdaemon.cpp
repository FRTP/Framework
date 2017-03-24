#include "cdaemon.h"

CServer::CServer(boost::shared_ptr<io_service> io_srvs)
	: m_acceptor(*io_srvs, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {
	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	CCommandFactory::add<server_command::CCmdGetFile>(ECommand::GET_FILE);
	CCommandFactory::add<server_command::CCmdGetMD5>(ECommand::GET_MD5);
	CCommandFactory::add<server_command::CCmdUploadFile>(ECommand::UPLOAD_FILE);

	_start_accept();	
}

void CServer::_start_accept() {
	BOOST_LOG_TRIVIAL(info) << "Waiting for client...";

	CTCPConnection::conn_ptr connection = CTCPConnection::conn_ptr(new CTCPConnection(m_acceptor.get_io_service()));
	m_acceptor.async_accept(connection->context().socket(), boost::bind(&CServer::_handle_accept, this, connection,
				placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	BOOST_LOG_TRIVIAL(info) << "Accepting client...";
	if (!ec) {
		BOOST_LOG_TRIVIAL(info) << "New client accepted";
		connection->context().async_recv_message(boost::bind(&CTCPConnection::handle_recv_message, connection,
							 placeholders::error));
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "Unable to accept client: " + ec.message();
	}
	_start_accept();
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
	CServer server(m_io_service);
	m_io_service->run();
	BOOST_LOG_TRIVIAL(info) << "CDaemon::start(): daemon stopped.";

	return 0;
}
