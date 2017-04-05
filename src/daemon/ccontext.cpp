#include "ccontext.h"

bool CContext::socket_opened() const {
	return m_socket.is_open();
}

boost::asio::ip::tcp::socket& CContext::socket() {
	return m_socket;
}

void CContext::rflush() {
	m_readbuf.consume(m_readbuf.size());
}

void CContext::wflush() {
	m_writebuf.consume(m_writebuf.size());
}

utility::EError CContext::send_message(const utility::CMessage& msg) {
	boost::system::error_code error;
	msg.to_streambuf(m_writebuf);
	boost::asio::write(m_socket, m_writebuf, error);
	wflush();
	if (error) {
		return utility::EError::WRITE_ERROR;
	}
	return utility::EError::OK;
}

utility::EError CContext::recv_message(utility::CMessage& msg) {
	boost::system::error_code error;
	boost::asio::read_until(m_socket, m_readbuf, utility::CMessage::MESSAGE_ENDING, error);
	if (error && error != boost::asio::error::eof) {
		rflush();
		return utility::EError::READ_ERROR;
	}
	utility::EError ret = msg.from_streambuf(m_readbuf);
	rflush();
	return ret;
}

utility::EError CContext::send_feedback(utility::EError error) {
	boost::system::error_code ec;
	std::vector<char> payload({ static_cast<char>(error) });
	utility::CMessage feedback(utility::ECommand::FEEDBACK, utility::EDataType::ERROR_CODE, payload);
	feedback.to_streambuf(m_writebuf);
	boost::asio::write(m_socket, m_writebuf, ec);
	wflush();
	if (ec) {
		return utility::EError::WRITE_ERROR;
	}
	return utility::EError::OK;
}

void CContext::async_send_message(const utility::CMessage& msg, callback_type callback) {
	msg.to_streambuf(m_writebuf);
	boost::asio::async_write(m_socket, m_writebuf, callback);
}

void CContext::async_recv_message(callback_type callback) {
	boost::asio::async_read_until(m_socket, m_readbuf, utility::CMessage::MESSAGE_ENDING, callback);
}

void CContext::async_send_feedback(utility::EError error, callback_type callback) {
	std::vector<char> payload({ static_cast<char>(error) });
	utility::CMessage feedback(utility::ECommand::FEEDBACK, utility::EDataType::ERROR_CODE, payload);
	feedback.to_streambuf(m_writebuf);
	boost::asio::async_write(m_socket, m_writebuf, callback);
}

utility::EError CContext::read_buffer(utility::CMessage& msg) {
	utility::EError ret = msg.from_streambuf(m_readbuf);
	rflush();
	return ret;
}

void CContext::close_socket(boost::system::error_code& ec) {
	if (m_socket.is_open()) {
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_socket.close();
	}
}

CContext::~CContext() {
	if (m_socket.is_open()) {
		m_socket.close();
	}
}
