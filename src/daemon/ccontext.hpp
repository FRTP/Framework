#ifndef CCONTEXT_HPP
#define CCONTEXT_HPP

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "utility.h"

using namespace boost::asio;
using namespace utility;

class CContext {
	protected:
		ip::tcp::socket m_socket;
		streambuf m_writebuf;
		streambuf m_readbuf;
	public:
		typedef boost::function<void(const boost::system::error_code&, std::size_t bytes_transferred)> callback_type;

		CContext(io_service& io_srvs) :
				m_socket(io_srvs) {}

		bool socket_opened() const {
			return m_socket.is_open();
		}
		
		ip::tcp::socket& socket() {
			return m_socket;
		}

		void rflush() {
			m_readbuf.consume(m_readbuf.size());
		}

		void wflush() {
			m_writebuf.consume(m_writebuf.size());
		}

		EError send_message(const CMessage& msg) {
			boost::system::error_code error;
			msg.to_streambuf(m_writebuf);
			write(m_socket, m_writebuf, error);
			wflush();
			if (error) {
				return EError::WRITE_ERROR;
			}
			return EError::OK;
		}

		EError recv_message(CMessage& msg) {
			boost::system::error_code error;
			read_until(m_socket, m_readbuf, CMessage::MESSAGE_ENDING, error);
			if (error && error != error::eof) {
				rflush();
				return EError::READ_ERROR;
			}
			EError ret = msg.from_streambuf(m_readbuf);
			rflush();
			return ret;
		}

		EError send_feedback(EError error) {
			boost::system::error_code ec;
			std::vector<char> payload({ static_cast<char>(error) });
			CMessage feedback(ECommand::FEEDBACK, EDataType::ERROR_CODE, payload);
			feedback.to_streambuf(m_writebuf);
			write(m_socket, m_writebuf, ec);
			wflush();
			if (ec) {
				return EError::WRITE_ERROR;
			}
			return EError::OK;
		}

		void async_send_message(const CMessage& msg, callback_type callback) {
			msg.to_streambuf(m_writebuf);
			async_write(m_socket, m_writebuf, callback);
		}

		void async_recv_message(callback_type callback) {
			async_read_until(m_socket, m_readbuf, CMessage::MESSAGE_ENDING, callback);
		}
		
		void async_send_feedback(EError error, callback_type callback) {
			std::vector<char> payload({ static_cast<char>(error) });
			CMessage feedback(ECommand::FEEDBACK, EDataType::ERROR_CODE, payload);
			feedback.to_streambuf(m_writebuf);
			async_write(m_socket, m_writebuf, callback);
		}

		void read_buffer(CMessage& msg) {
			msg.from_streambuf(m_readbuf);
			rflush();
		}

		~CContext() {
			if (m_socket.is_open()) {
				m_socket.close();
			}
		}
};

#endif //CCONTEXT_HPP
