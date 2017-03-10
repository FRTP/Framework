#include "cdaemon.h"

CServer::CServer(boost::asio::io_service& io_service)
	: m_acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {

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
		boost::asio::async_read(connection->socket(), m_readbuf,
					boost::bind(&CTCPConnection::handle_read_command, connection, command,
					boost::asio::placeholders::error));
		_start_accept();
	}
}

void CTCPConnection::handle_read_command(const boost::system::error_code& ec) {
	BOOST_LOG_TRIVIAL(info) << "New client accepted; reading command...";

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
			boost::asio::async_read(m_socket, m_readbuf,
						boost::bind(&CTCPConnection::handle_read_filename, shared_from_this(),
						srv_cmd, e_datatype, boost::asio::placeholders::error));
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

	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ *filename }));
	if (!datatype_instance->success()) {
		BOOST_LOG_TRIVIAL(error) << "Ivalid argument list for data type instance";
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
			EError ret = datatype_instance->get_data(data_buf);
			delete datatype_instance;
			if (ret != EError::OK) {
				m_readbuf.consume(m_readbuf.size());
				std::ostream out(&m_writebuf);
				out << static_cast<int>(ret) << std::endl;

				async_write(m_socket, m_writebuf,
					    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
					    placeholders::error));
				return;
			}
			std::ostream out(&m_writebuf);
			out << data_buf << std::endl;

			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    placeholders::error));
			break;
		case ECommand::GET_MD5:
			md5sum_ptr md5 = calculate_md5(*filename);
			std::ostream out(&m_writebuf);
			out << *md5 << std::endl;

			async_write(m_socket, m_writebuf,
				    boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
				    placeholders::error));
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
		BOOST_LOG_TRIVIAL(error) << "handle_transfer_file: " + ec.message();
		return;
	}
	const char* data = buffer_cast<const char*>(m_readbuf.data());
	datatype_instance->write_data(data, m_readbuf.size());
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

CTCPConnection::conn_ptr CTCPConnection::create(boost::asio::io_service& io_service) {
	return conn_ptr(new CTCPConnection(io_service));
}

ip::tcp::socket& CTCPConnection::socket() {
	return m_socket;
}

CDaemon::CDaemon(const CParser& parser) : m_io_service(new io_service()) {
	CSettings::set_working_dir("/var/frtp/");
	logging::add_file_log(CSettings::working_dir() + "frtp.log");
}

int CDaemon::start() {
	CServer server(*(m_io_service.get()));
	boost::asio::signal_set signals(*(m_io_service.get()), SIGINT, SIGTERM);
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
