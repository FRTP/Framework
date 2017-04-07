#ifndef CDAEMON_H
#define CDAEMON_H

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <openssl/md5.h>
#include <string>

#include "datatype.h"
#include "cparser.hpp"
#include "ctcpconnection.hpp"
#include "../utility.h"

namespace fs = boost::filesystem;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

class CServer {
	private:
		static constexpr int PORT = 34567;
		static constexpr int MAX_CONNECTIONS = 5;

		boost::asio::ip::tcp::acceptor m_acceptor;

		void _start_accept();
		void _handle_accept(CTCPConnection::conn_ptr connection, const boost::system::error_code& ec);
	public:
		explicit CServer(boost::shared_ptr<boost::asio::io_service> io_srvs);
};

class CDaemon {
	private:
		boost::shared_ptr<boost::asio::io_service> m_io_service;
	public:
		explicit CDaemon(const CParser& parser);
		int start();
};

#endif //CDAEMON_H
