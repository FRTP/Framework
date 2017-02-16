#ifndef CDAEMON_H
#define CDAEMON_H

#include <string>
#include <memory>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include "clog.h"
#include "exception.hpp"

namespace fs = boost::filesystem;
using namespace boost::asio;

class CServer {
	public:
		enum ECommand {
			GET_FILE,
			GET_FILE_LIST
		};

		class CTCPConnection : public boost::enable_shared_from_this<CTCPConnection> {
			private:
				tcp::socket m_socket;
			public:
				typedef boost::shared_ptr<CTCPConnection> conn_ptr;

				static conn_ptr create(boost::asio::io_service& io_service);
				tcp::socket& socket();
		};
	private:
		static const int PORT = 34567;
		static const int MAX_CONNECTIONS = 5;
		static std::string m_data_path = "/var/frtp/data/";

		ip::tcp::acceptor m_acceptor;
		std::vector<int> m_command(2);
		std::string m_filename;
		std::shared_ptr<CLog> m_log;

		void _start_accept();
		void _handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec);
		void _handle_read_command(const boost::system::error_code& ec);
		void _handle_read_filename(const boost::system::error_code& ec);
		void _handle_write_response(const boost::system::error_code& ec);
	public:
		CServer(boost::asio::io_service& io_service, const std::shared_ptr<CLog>& log);
};

class CDaemon {
	private:
		static const std::string DEFAULT_CONFIG = "~/.config/frtp/main.conf";

		std::shared_ptr<CLog> m_log;
	public:
		CDaemon(const CParser& parser);
		int start();
};

#endif //CDAEMON_H
