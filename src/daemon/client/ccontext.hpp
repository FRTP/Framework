#ifndef CCONTEXT_HPP
#define CCONTEXT_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

#include "../utility.h"

using namespace boost::asio;
using namespace utility;

class CContext {
	private:
		std::string m_server;
		int m_port;
		ip::tcp::socket m_socket;
		streambuf m_writebuf;
	public:
		CContext(const std::string& server, int port, boost::shared_ptr<io_service> io_service)
			: m_server(server), m_port(port), m_socket(*(io_service.get())) {}

		void connect(boost::system::error_code& error) {
			m_socket.connect(ip::tcp::endpoint(ip::address::from_string(m_server.c_str()), m_port), error);
		}

		bool socket_opened() const {
			return m_socket.is_open();
		}

		template<class T>
		EError socket_write(const T& msg) {
			boost::system::error_code error;
			std::ostream out(&m_writebuf);
			for (auto i : msg) {
				out << i << " ";
			}
			out << std::endl;
			write(m_socket, m_writebuf, error);
			m_writebuf.consume(m_writebuf.size());
			if (error) {
				return EError::WRITE_ERROR;
			}
			return EError::OK;
		}

		EError socket_read(streambuf& buffer) {
			boost::system::error_code error;
			read_until(m_socket, buffer, '\n', error);
			if (error && error != error::eof) {
				return EError::READ_ERROR;
			}
			return EError::OK;
		}

		~CContext() {
			m_socket.close();
		}
};

template<>
inline EError CContext::socket_write<std::string>(const std::string& msg) {
	boost::system::error_code error;
	std::ostream out(&m_writebuf);
	out << msg << " " << std::endl;
	write(m_socket, m_writebuf, error);
	m_writebuf.consume(m_writebuf.size());
	if (error) {
		return EError::WRITE_ERROR;
	}
	return EError::OK;
}

#endif //CCONTEXT_HPP
