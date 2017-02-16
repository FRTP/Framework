#include "cdaemon.h"

CServer::CServer(boost::asio::io_service& io_service, const std::shared_ptr<CLog>& log)
	: m_acceptor(ip::tcp::v4(), PORT) {
	m_log = log;
	_start_accept();	
}

void CServer::_start_accept() {
	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.io_service());
	m_acceptor.async_accept(connection->socket(), boost::bind(&CServer::_handle_accept, this, connection, 
				boost::asio::placeholders::error));
}

void CServer::_handle_accept(CServer::CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if(!ec) {
		boost::asio::async_read(connection->socket(), boost::asio::buffer(m_command), 
				boost::bind(&CTCPConnection::_handle_read_command, shared_from_this(), connection,
				boost::asio::placeholders::error));
		_start_accept();
	}
}

void CServer::_handle_read_command(CServer::CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if(!ec) {
		if(m_command[0] == ECommand::GET_FILE) {
			if(m_command < 0) {
				m_log->write("[EE]: Invalid buffer size");
				return;
			}
			m_filename.resize(m_command[1]);
			boost::asio::async_read(connection->socket(), boost::asio::buffer(&m_filename[0], m_filename.size), 
				boost::bind(&CTCPConnection::_handle_read_filename, shared_from_this(), connection,
				boost::asio::placeholders::error));
		}
		else if(m_command[0] == ECommand::GET_FILE_LIST) {
			std::string files = "";
			for(fs::recursive_directory_iterator it(m_data_path), end; it != end; ++it)
				files += it->path().filename();
			boost::asio::async_write(connection->socket(), boost::asio::buffer(&files[0], files.size()), 
				boost::bind(&CTCPConnection::_handle_write_response, shared_from_this(),
				boost::asio::placeholders::error));
		}
	}
	else
		m_log->write("[EE]: " + ec.message());
}

void CServer::_handle_read_filename(CServer::CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if(ec) {
		m_log->write("[EE]: " + ec.message());
		return;
	}
	std::ifstream file(m_filename, std::ios::binary);
	if(!file) {
		m_log->write("[EE]: Unable to open file " + m_filename);
		return;
	}
	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);

	std::vector<char> buffer(size);
	if(file.read(buffer.data(), size)) {
		boost::asio::async_write(connection->socket(), boost::asio::buffer(buffer), 
				boost::bind(&CTCPConnection::_handle_write_response, shared_from_this(),
				boost::asio::placeholders::error));
	}
	else
		m_log->write("[EE]: Unable to read file " + filename);
	file.close();
}

void CServer::_handle_write_response(const boost::system::error_code& ec) {
	if(!ec) 
		m_log->write("[II]: Data transmitted successfully");
	else
		m_log->write("[EE]: " + ec.message());
}

CServer::CTCPConnection::conn_ptr CServer::CTCPConnection::create(boost::asio::io_service& io_service) {
	return conn_ptr(new CTCPConnection(io_service));
}

tcp::socket& CServer::CTCPConnection::socket() {
	return m_socket;
}

CDaemon::CDaemon(const CParser& parser) {
	m_log.replace(new CLog(parser.log_type()));

	std::string config_name;
	if(!parser.config(config_name))
		config_name = DEFAULT_CONFIG;
	if(!_load_config(config_name))
		m_log->write("[EE]: Config file " + config_name + " not found.");
}

void CDaemon::_set_pid_file(const std::string& filename) {
	std::ofstream out(filename);
	out << getpid();
	out.close();
}

int CDaemon::start() {
	boost::asio::io_service io_service;
	CServer server(io_service, m_log);
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));
	io_service.notify_fork(boost::asio:io_service::fork_prepare);

	if(pid_t pid = fork()) {
		if(pid > 0) {
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
	if(pid_t pid = fork()) {
		if(pid > 0) 
			exit(0);
		else {
			m_log->write("[EE]: Second fork failed.");
			return 1;
		}
	}
	close(0);
	close(1);
	close(2);

	if(open("/dev/null", O_RDONLY) < 0) {
		m_log->write("[EE]: Unable to open /dev/null.");
		return 1;
	}

	io_service.notify_fork(boost::asio::io_service::fork_child);
	m_log->write("[II]: Daemon started.");
	io_service.run();
	m_log->write("[II]: Daemon stopped.");
}
