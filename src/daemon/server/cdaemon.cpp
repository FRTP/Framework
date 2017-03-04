#include "cdaemon.h"

CServer::CServer(boost::asio::io_service& io_service, const boost::shared_ptr<CLog>& log)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {
	m_log = log;
	_start_accept();	
}

void CServer::_start_accept() {
	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.get_io_service());
	m_acceptor.async_accept(connection->socket(), boost::bind(&CServer::_handle_accept, this, connection, 
				boost::asio::placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if (!ec) {
		boost::shared_ptr<std::array<int, 2>> command(new std::array<int, 2>);
		boost::asio::async_read(connection->socket(), boost::asio::buffer(*command), 
				boost::bind(&CTCPConnection::handle_read_command, connection, m_log, command, 
				boost::asio::placeholders::error));
		_start_accept();
	}
}

void CTCPConnection::handle_read_command(boost::shared_ptr<CLog> log, boost::shared_ptr<std::array<int, 2>> command, 
		const boost::system::error_code& ec) {
	if (!ec) {
		log->write("[II]: New client accepted; reading command...");
		if (static_cast<CServer::ECommand>((*command)[0]) == CServer::ECommand::GET_FILE) {
			if ((*command)[1] < 0) {
				log->write("[EE] handle_read_command: Invalid buffer size");
				return;
			}
			boost::shared_ptr<std::string> filename(new std::string);
			filename->resize((*command)[1]);
			boost::asio::async_read(m_socket, boost::asio::buffer(&(*filename)[0], filename->size()), 
				boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(), log, filename,
				static_cast<int>(CServer::ECommand::GET_FILE), boost::asio::placeholders::error));
		}
		else if (static_cast<CServer::ECommand>((*command)[0]) == CServer::ECommand::GET_MD5) {
			if ((*command)[1] < 0) {
				log->write("[EE] handle_read_command: Invalid buffer size");
				return;
			}
			boost::shared_ptr<std::string> filename(new std::string);
			filename->resize((*command)[1]);
			boost::asio::async_read(m_socket, boost::asio::buffer(&(*filename)[0], filename->size()),
				boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(), log, filename,
				static_cast<int>(CServer::ECommand::GET_MD5), boost::asio::placeholders::error));
		}
	}
	else {
		log->write("[EE]: handle_read_command: " + ec.message());
	}
}

void CTCPConnection::handle_read_filename(boost::shared_ptr<CLog>& log, boost::shared_ptr<std::string> filename, 
		int command, const boost::system::error_code& ec) {
	if (ec) {
		log->write("[EE]: " + ec.message());
		return;
	}

	std::string s_filename("/var/frtp/data/" + *filename);
	std::ifstream file(s_filename, std::ios::binary);
	if (!file) {
		log->write("[EE]: Unable to open file " + s_filename + ": " + strerror(errno));
		int error_buf[1] = { CServer::EError::NO_FILE };
		boost::asio::async_write(m_socket, boost::asio::buffer(error_buf), 
				boost::bind(&CTCPConnection::handle_write_response, shared_from_this(), log,
				boost::asio::placeholders::error));
		return;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	std::vector<char> buffer(size);

	if (!file.read(buffer.data(), size)) {
		log->write("[EE]: Unable to read file " + s_filename);
		file.close();
		return;
	}

	if (static_cast<CServer::ECommand>(command) == CServer::ECommand::GET_FILE) {
		boost::asio::async_write(m_socket, boost::asio::buffer(buffer), 
				boost::bind(&CTCPConnection::handle_write_response, shared_from_this(), log,
				boost::asio::placeholders::error));
	}
	else if (static_cast<CServer::ECommand>(command) == CServer::ECommand::GET_MD5) {
		unsigned char md5sum[MD5_DIGEST_LENGTH];
		MD5((unsigned char*)buffer.data(), size, md5sum);
		boost::asio::async_write(m_socket, boost::asio::buffer(buffer),
				boost::bind(&CTCPConnection::handle_write_response, shared_from_this(), log,
				boost::asio::placeholders::error));
	}

	file.close();
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

CDaemon::CDaemon(const CParser& parser) {
	m_log = boost::shared_ptr<CLog>(new CLog(parser.logname()));

	/*
	std::string config_name = parser.configname();
	if (!_load_config(config_name)) {
		m_log->write("[EE]: Config file " + config_name + " not found.");
	}
	*/
}

int CDaemon::start() {
	boost::asio::io_service io_service;
	CServer server(io_service, m_log);
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));
	io_service.notify_fork(boost::asio::io_service::fork_prepare);

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

	io_service.notify_fork(boost::asio::io_service::fork_child);
	m_log->write("[II]: Daemon started.");
	io_service.run();
	m_log->write("[II]: Daemon stopped.");

	return 0;
}
