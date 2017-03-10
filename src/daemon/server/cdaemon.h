#ifndef CDAEMON_H
#define CDAEMON_H

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <openssl/md5.h>
#include <string>

#include "../datatype.h"
#include "cparser.hpp"
#include "../utility.hpp"

namespace fs = boost::filesystem;
namespace logging = boost::log;
using namespace boost::asio;
using namespace datatypes;
using namespace utility;

class CTCPConnection : public boost::enable_shared_from_this<CTCPConnection> {
	private:
		ip::tcp::socket m_socket;
		streambuf m_readbuf;
		streambuf m_writebuf;
		explicit CTCPConnection(boost::asio::io_service& io_service) : m_socket(io_service) {}
	public:
		typedef boost::shared_ptr<CTCPConnection> conn_ptr;
		void handle_read_command(const boost::system::error_code& ec);
		void handle_read_filename(ECommand command, EDataType datatype, const boost::system::error_code& ec);
		void handle_transfer_file(const std::string& filename, IDataType* datatype_instance,
					  const boost::sytem::error_code& ec);
		void handle_write_response(const boost::system::error_code& ec);
		static conn_ptr create(boost::asio::io_service& io_service);
		ip::tcp::socket& socket();
};

class CServer {
	private:
		static constexpr int PORT = 34567;
		static constexpr int MAX_CONNECTIONS = 5;

		ip::tcp::acceptor m_acceptor;

		void _start_accept();
		void _handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec);
	public:
		CServer(io_service& io_service);
};

class CDaemon {
	private:
		boost::shared_ptr<io_service> m_io_service;
	public:
		explicit CDaemon(const CParser& parser);
		int start();
};

#endif //CDAEMON_H
