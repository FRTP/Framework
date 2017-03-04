#ifndef CCONTEXT_HPP
#define CCONTEXT_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;

class CContext {
	public:
		enum EError {
			OK,
			WRITE_ERROR,
			READ_ERROR, 
			SOCKET_ERROR
		};
	private:
		std::string m_server;
		int m_port;
		ip::tcp::socket m_socket;
	public:
		CContext(const std::string& server, int port, io_service& io_service) 
			: m_server(server), m_port(port), m_socket(io_service) {}
		void connect(boost::system::error_code& error) {
			m_socket.connect(ip::tcp::endpoint(ip::address::from_string(m_server.c_str()), m_port), error);
		}
		bool socket_opened() const {
			return m_socket.is_open();
		}
		template<class T>
		EError socket_write(T msg, int buf_size) {
			boost::system::error_code error;
			write(m_socket, buffer(msg, buf_size), error);
			if (error) {
				return EError::WRITE_ERROR;
			}
			return EError::OK;
		}
		EError socket_read(streambuf& buffer) {
			boost::system::error_code error;
			read(m_socket, buffer, transfer_all(), error);
			if (error && error != error::eof) {
				return EError::READ_ERROR;
			}
			return EError::OK;
		}
		~CContext() {
			m_socket.close();
		}
};

#endif //CCONTEXT_HPP
