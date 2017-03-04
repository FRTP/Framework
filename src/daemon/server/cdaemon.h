#ifndef CDAEMON_H
#define CDAEMON_H

#include <array>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <memory>
#include <openssl/md5.h>
#include <string>

#include "clog.hpp"
#include "cparser.hpp"

namespace fs = boost::filesystem;
using namespace boost::asio;

class CTCPConnection : public boost::enable_shared_from_this<CTCPConnection> {
	private:
		ip::tcp::socket m_socket;
		explicit CTCPConnection(boost::asio::io_service& io_service) : m_socket(io_service) {}
	public:
		typedef boost::shared_ptr<CTCPConnection> conn_ptr;
		void handle_read_command(boost::shared_ptr<CLog> log, boost::shared_ptr<std::array<int, 2>> command, 
				const boost::system::error_code& ec);
		void handle_read_filename(boost::shared_ptr<CLog>& log, boost::shared_ptr<std::string> filename, 
				int command, const boost::system::error_code& ec);
		void handle_write_response(boost::shared_ptr<CLog>& log, const boost::system::error_code& ec);
		static conn_ptr create(boost::asio::io_service& io_service);
		ip::tcp::socket& socket();
};

class CServer {
	public:
		enum ECommand {
			GET_FILE,
			GET_MD5
		};
		enum EError {
			NO_FILE
		};
	private:
		static const int PORT = 34567;
		static const int MAX_CONNECTIONS = 5;

		ip::tcp::acceptor m_acceptor;
		boost::shared_ptr<CLog> m_log;

		void _start_accept();
		void _handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec);
	public:
		CServer(io_service& io_service, const boost::shared_ptr<CLog>& log);
};

class CDaemon {
	private:
		boost::shared_ptr<CLog> m_log;
	public:
		explicit CDaemon(const CParser& parser);
		int start();
};

#endif //CDAEMON_H
