#include "cdaemon.h"

CServer::CServer(boost::asio::io_service& io_service, const boost::shared_ptr<CLog>& log)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {
	m_log = log;

	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	_start_accept();	
}

void CServer::_start_accept() {
	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.get_io_service());
	m_acceptor.async_accept(connection->socket(), boost::bind(&CServer::_handle_accept, this, connection, 
				boost::asio::placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if (!ec) {
		boost::shared_ptr<std::array<int, 3>> command(new boost::array<int, 3>);
		boost::asio::async_read(connection->socket(), boost::asio::buffer(*command), 
					boost::bind(&CTCPConnection::handle_read_command, connection, m_log, command,
					boost::asio::placeholders::error));
		_start_accept();
	}
}

void CTCPConnection::handle_read_command(boost::shared_ptr<CLog> log, boost::shared_ptr<std::array<int, 3>> command,
					 const boost::system::error_code& ec) {
	log->write("[II]: New client accepted; reading command...");
	ECommand srv_cmd = static_cast<ECommand>((*command)[0]);
	int filename_size = (*command)[1];
	int datatype = (*command)[2];

	if (!ec) {
		if (srv_cmd == ECommand::GET_FILE ||
		    srv_cmd == ECommand::GET_MD5) {
			std::string s_cmd = (srv_cmd == ECommand::GET_FILE) ? "GET_FILE" : "GET_MD5";
			log->write("[II]: Trying to process " + s_cmd + " command");

			if (filename_size < 0) {
				log->write("[EE] handle_read_command: Invalid buffer size");
				return;
			}
			if (datatype < 0 || datatype > static_cast<int>(EDataType::MAX_VAL)) {
				log->write("[EE] handle_read_command: Invalid data type");
				return;
			}

			EDataType e_datatype = static_cast<EDataType>(datatype);
			boost::shared_ptr<std::string> filename(new std::string);
			filename->resize(filename_size);
			boost::asio::async_read(m_socket, boost::asio::buffer(&(*filename)[0], filename_size),
						boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(),
						log, filename, srv_cmd, e_datatype, boost::asio::placeholders::error));
		}
	}
	else {
		log->write("[EE]: handle_read_command: " + ec.message());
	}
}

void CTCPConnection::handle_read_filename(boost::shared_ptr<CLog>& log, boost::shared_ptr<std::string> filename, 
					  ECommand command, EDataType datatype, const boost::system::error_code& ec) {
	if (ec) {
		log->write("[EE]: " + ec.message());
		return;
	}

	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ *filename }));
	std::vector<char> data_buf;
	EError ret = datatype_instance->get_data(data_buf, log);
	delete datatype_instance;

	if (ret != EError::OK) {
		int error_buf[1] = { static_cast<int>(ret) };
		boost::asio::async_write(m_socket, boost::asio::buffer(error_buf),
					 boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
					 log, boost::asio::placeholders::error));
		return;
	}

	switch (command) {
		case ECommand::GET_FILE:
			boost::asio::async_write(m_socket, boost::asio::buffer(data_buf),
						 boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
						 log, boost::asio::placeholders::error));
			break;
		case ECommand::GET_MD5:
			unsigned char md5sum[MD5_DIGEST_LENGTH];
			MD5((unsigned char*)data_buf.data(), size, md5sum);
			boost::asio::async_write(m_socket, boost::asio::buffer(md5sum),
						 boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
						 log, boost::asio::placeholders::error));
			break;
	}
}

void CTCPConnection::handle_write_response(boost::shared_ptr<CLog>& log, const boost::system::error_code& ec) {
	if (!ec) { 
		log->write("[II]: Response transmitted successfully");
	}
	else {
		log->write("[EE]: " + ec.message());
	}
}

CTCPConnection::conn_ptr CTCPConnection::create(boost::asio::io_service& io_service) {
	return conn_ptr(new CTCPConnection(io_service));
}

ip::tcp::socket& CTCPConnection::socket() {
	return m_socket;
}

CDaemon::CDaemon(const CParser& parser) : m_io_service(new io_service()) {
	m_log = boost::shared_ptr<CLog>(new CLog(parser.logname()));
}

int CDaemon::start() {
	CServer server(*(m_io_service.get()), m_log);
	boost::asio::signal_set signals(*(m_io_service.get()), SIGINT, SIGTERM);
	signals.async_wait(boost::bind(&io_service::stop, m_io_service));
	m_io_service->notify_fork(io_service::fork_prepare);

	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		}
		else {
			m_log->write("[EE]: First fork failed.");
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
			m_log->write("[EE]: Second fork failed.");
			return 1;
		}
	}
	close(0);
	close(1);
	close(2);

	if (open("/dev/null", O_RDONLY) < 0) {
		m_log->write("[EE]: Unable to open /dev/null.");
		return 1;
	}

	m_io_service->notify_fork(io_service::fork_child);
	m_log->write("[II]: Daemon started.");
	m_io_service->run();
	m_log->write("[II]: Daemon stopped.");

	return 0;
}
