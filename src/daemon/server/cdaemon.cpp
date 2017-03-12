#include "cdaemon.h"

CServer::CServer(io_service& io_service)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {

	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	_start_accept();	
}

void CServer::_start_accept() {
	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.get_io_service());
	m_acceptor.async_accept(connection->m_socket, boost::bind(&CServer::_handle_accept, this, connection, 
				placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if (!ec) {
		async_read(connection->m_socket, connection->m_readbuf,
			   boost::bind(&CTCPConnection::handle_read_command, connection,
			   placeholders::error));
		_start_accept();
	}
}

void CTCPConnection::handle_read_command(const boost::system::error_code& ec) {
	BOOST_LOG_TRIVIAL(info) << "New client accepted; reading command...";

	int srv_cmd_int;
	int filename_size;
	int datatype;

	m_in >> srv_cmd_int >> filename_size >> datatype;
	m_readbuf.consume(m_readbuf.size());

	ECommand srv_cmd = static_cast<ECommand>(srv_cmd_int);
	if (!ec) {
		if (srv_cmd == ECommand::GET_FILE ||
		    srv_cmd == ECommand::GET_MD5 ||
		    srv_cmd == ECommand::UPLOAD_FILE) {
			std::string s_cmd;
			switch (srv_cmd) {
				case ECommand::GET_FILE:
					s_cmd = "GET_FILE";
					break;
				case ECommand::GET_MD5:
					s_cmd = "GET_MD5";
					break;
				case ECommand::UPLOAD_FILE:
					s_cmd = "UPLOAD_FILE";
					break;
			}
			BOOST_LOG_TRIVIAL(info) << "Trying to process " + s_cmd + " command";

			if (filename_size < 0) {
				BOOST_LOG_TRIVIAL(error) << "handle_read_command: Invalid buffer size";
				return;
			}
			if (datatype < 0 || datatype > static_cast<int>(EDataType::MAX_VAL)) {
				BOOST_LOG_TRIVIAL(error) << "handle_read_command: Invalid data type";
				return;
			}

			EDataType e_datatype = static_cast<EDataType>(datatype);
			async_read(m_socket, m_readbuf,
				   boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(),
				   srv_cmd, e_datatype, placeholders::error));
		}
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "handle_read_command: " + ec.message();
	}
}

void CTCPConnection::handle_read_filename(ECommand command, EDataType datatype,
					  const boost::system::error_code& ec) {
	if (ec) {
		BOOST_LOG_TRIVIAL(error) << "handle_read_filename: " + ec.message();
		m_readbuf.consume(m_readbuf.size());
		return;
	}
	std::string filename;
	m_in >> filename;
	m_readbuf.consume(m_readbuf.size());

	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ filename }));
	if (!datatype_instance->success()) {
		BOOST_LOG_TRIVIAL(error) << "Ivalid argument list for data type instance";
		m_readbuf.consume(m_readbuf.size());
		return;
	}

	std::vector<char> data_buf;
	EError ret;
	switch (command) {
		case ECommand::GET_FILE:
			if ((ret = datatype_instance->get_data(data_buf)) != EError::OK) {
				delete datatype_instance;
				m_readbuf.consume(m_readbuf.size());
				m_out << static_cast<int>(ret) << std::endl;

				async_write(m_socket, m_writebuf,
					    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
					    placeholders::error));
				return;
			}
			delete datatype_instance;
			_send_container<std::vector<char>>(data_buf);
			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    placeholders::error));
			break;
		case ECommand::GET_MD5:
			_send_container<md5sum>(*(calculate_md5(filename)));
			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    placeholders::error));
			break;
		case ECommand::UPLOAD_FILE:
			async_read_until(m_socket, m_readbuf, '\n',
					 boost::bind(&CTCPConnection::handle_transfer_file, shared_from_this(),
					 datatype_instance, placeholders::error));
			break;

	}
}

void CTCPConnection::handle_transfer_file(IDataType* datatype_instance, const boost::system::error_code& ec) {
	if (ec && ec != error::eof) {
		BOOST_LOG_TRIVIAL(error) << "handle_transfer_file: " + ec.message();
		return;
	}
	const char* data = buffer_cast<const char*>(m_readbuf.data());
	datatype_instance->write_data(std::vector<char>(data, data + m_readbuf.size()));
	m_readbuf.consume(m_readbuf.size());
	delete datatype_instance;
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
	logging::add_file_log(parser.logname());
}

int CDaemon::start() {
	CServer server(*(m_io_service.get()));
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
	m_io_service->run();
	BOOST_LOG_TRIVIAL(info) << "CDaemon::start(): daemon stopped.";

	return 0;
}
