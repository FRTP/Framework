#include "cdaemon.h"

CServer::CServer(boost::asio::io_service& io_service, const boost::shared_ptr<CLog>& log)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {
	m_log = log;

	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	CSettings::set_working_dir("/var/frtp/");

	_start_accept();	
}

void CServer::_start_accept() {
	CTCPConnection::conn_ptr connection = CTCPConnection::create(m_acceptor.get_io_service());
	m_acceptor.async_accept(connection->socket(), boost::bind(&CServer::_handle_accept, this, connection, 
				boost::asio::placeholders::error));
}

void CServer::_handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec) {
	if (!ec) {
		boost::asio::async_read(connection->socket(), m_readbuf,
					boost::bind(&CTCPConnection::handle_read_command, connection, m_log, command,
					boost::asio::placeholders::error));
		_start_accept();
	}
}

void CTCPConnection::handle_read_command(boost::shared_ptr<CLog> log, const boost::system::error_code& ec) {
	log->write("[II]: New client accepted; reading command...");
	ECommand srv_cmd;
	int filename_size;
	int datatype;

	std::istream in(&m_readbuf);
	in >> srv_cmd >> filename_size >> datatype;
	m_readbuf.consume(m_readbuf.size());

	if (!ec) {
		if (srv_cmd == ECommand::GET_FILE ||
		    srv_cmd == ECommand::GET_MD5 ||
		    srv_cmd == ECommand::UPLOAD_FILE) {
			std::string s_cmd;
			switch (srv_cmd) {
				case ECommand::GET_FILE:
					s_smd = "GET_FILE";
					break;
				case ECommand::GET_MD5:
					s_cmd = "GET_MD5";
					break;
				case ECommand::UPLOAD_FILE:
					s_cmd = "UPLOAD_FILE";
					break;
			}
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
			boost::asio::async_read(m_socket, m_readbuf,
						boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(),
						log, srv_cmd, e_datatype, boost::asio::placeholders::error));
		}
	}
	else {
		log->write("[EE]: handle_read_command: " + ec.message());
	}
}

void CTCPConnection::handle_read_filename(boost::shared_ptr<CLog>& log, ECommand command, EDataType datatype,
					  const boost::system::error_code& ec) {
	if (ec) {
#include <iostream>
		log->write("[EE]: " + ec.message());
		m_readbuf.consume(m_readbuf.size());
		return;
	}

	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ *filename }));
	if (!datatype_instance->success()) {
		log->write("[EE]: Ivalid argument list for data type instance");
		m_readbuf.consume(m_readbuf.size());
		return;
	}

	std::string filename;
	std::istream in(&m_readbuf);
	in >> filename;
	m_readbuf.consume(m_readbuf.size());
	switch (command) {
		case ECommand::GET_FILE:
			std::vector<char> data_buf;
			EError ret = datatype_instance->get_data(data_buf, log);
			delete datatype_instance;
			if (ret != EError::OK) {
				m_readbuf.consume(m_readbuf.size());
				std::ostream out(&m_writebuf);
				out << static_cast<int>(ret) << std::endl;

				async_write(m_socket, m_writebuf,
					    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
					    log, placeholders::error));
				return;
			}
			std::ostream out(&m_writebuf);
			out << data_buf << std::endl;

			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    log, placeholders::error));
			break;
		case ECommand::GET_MD5:
			md5sum_ptr md5 = calculate_md5(*filename);
			std::ostream out(&m_writebuf);
			out << *md5 << std::endl;

			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    log, placeholders::error));
			break;
		case ECommand::UPLOAD_FILE:
			async_read_until(m_socket, m_readbuf, '\n',
					 boost::bind(&CTCPConnection::handle_transfer_file, shared_from_this(),
					 filename, datatype_instance, placeholders::error));
			break;

	}
}

void CTCPConnection::handle_transfer_file(const std::string& filename, IDataType* datatype_instance,
					  const boost::sytem::error_code& ec) {
	if (ec && ec != ec::eof) {
		//TODO: log error
		return;
	}
	const char* data = buffer_cast<const char*>(m_readbuf.data());
	datatype_instance->write_data(data, m_readbuf.size());
	m_readbuf.consume(m_readbuf.size());
	delete datatype_instance;
}


void CTCPConnection::handle_write_response(boost::shared_ptr<CLog>& log, const boost::system::error_code& ec) {
	m_writebuf.consume(m_writebuf.size());
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
