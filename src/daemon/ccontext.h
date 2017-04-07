#ifndef CCONTEXT_H
#define CCONTEXT_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "utility.h"

class CContext {
	protected:
		boost::asio::ip::tcp::socket m_socket;
		boost::asio::streambuf m_writebuf;
		boost::asio::streambuf m_readbuf;
	public:
		typedef boost::function<void(const boost::system::error_code&, std::size_t bytes_transferred)> callback_type;

		CContext(boost::asio::io_service& io_srvs) :
				m_socket(io_srvs) {}
		bool socket_opened() const;
		boost::asio::ip::tcp::socket& socket();
		void rflush();
		void wflush();
		utility::EError send_message(const utility::CMessage& msg);
		utility::EError recv_message(utility::CMessage& msg);
		utility::EError send_feedback(utility::EError error);
		void async_send_message(const utility::CMessage& msg, callback_type callback);
		void async_recv_message(callback_type callback);
		void async_send_feedback(utility::EError error, callback_type callback);
		utility::EError read_buffer(utility::CMessage& msg);
		void close_socket(boost::system::error_code& ec);
		~CContext();
};

#endif //CCONTEXT_H
