#ifndef CTCPCONNECTION_HPP
#define CTCPCONNECTION_HPP

#include <boost/system/error_code.hpp>
#include <boost/pointer_cast.hpp>

#include "ccommand.h"
#include "../ccontext.hpp"

class CTCPConnection : public boost::enable_shared_from_this<CTCPConnection> {
	private:
		CContext m_context;
		bool m_authorized;

		void _process_message(boost::shared_ptr<CMessage> msg) {
			auto cmd = boost::dynamic_pointer_cast<server_command::IServerCommand>(CCommandFactory::create(*msg));
			if (cmd == NULL) {
				m_context.async_send_feedback(EError::UNKNOWN_COMMAND, 
							      boost::bind(&CTCPConnection::handle_write_response, 
							      shared_from_this(), placeholders::error));
				return;
			}
			cmd->set_callback(boost::bind(&CTCPConnection::handle_write_response, shared_from_this(),
					  placeholders::error));
			EError ret;
			if ((ret = cmd->invoke(&m_context, msg->datatype())) != EError::OK) {
				m_context.async_send_feedback(ret, boost::bind(&CTCPConnection::handle_write_response, 
							      shared_from_this(), placeholders::error));
			}
		}
	public:
		typedef boost::shared_ptr<CTCPConnection> conn_ptr;

		explicit CTCPConnection(io_service& io_srvs) 
			: m_context(io_srvs), m_authorized(false) {}

		void authorize(const boost::system::error_code& ec) {
			BOOST_LOG_TRIVIAL(info) << "Trying to authorize...";
			if (!ec || ec == error::eof) {
				boost::shared_ptr<CMessage> msg(new CMessage);
				m_context.read_buffer(*msg);
				auto cmd = boost::dynamic_pointer_cast<server_command::IServerCommand>(CCommandFactory::create(*msg));
				if (cmd == NULL) {
					m_context.async_send_feedback(EError::UNKNOWN_COMMAND,
								      boost::bind(&CTCPConnection::handle_write_response,
								      shared_from_this(), placeholders::error));
					return;
				}
				else if (cmd->type() != ECommand::AUTHORIZE) {
					BOOST_LOG_TRIVIAL(warning) << "Invoked command without authorization! Disconnecting";
					m_context.async_send_feedback(EError::AUTH_ERROR,
								      boost::bind(&CTCPConnection::handle_write_response,
								      shared_from_this(), placeholders::error));
					return;
				}
				EError ret;
				if ((ret = cmd->invoke(&m_context, msg->datatype())) == EError::OK) {
					m_authorized = true;
				}
			}
		}

		void handle_recv_message(const boost::system::error_code& ec) {
			BOOST_LOG_TRIVIAL(info) << "Receiving message...";
			if (!ec || ec == error::eof) {
				BOOST_LOG_TRIVIAL(info) << "Processing incoming message...";
				boost::shared_ptr<CMessage> msg(new CMessage);
				m_context.read_buffer(*msg);
				_process_message(msg);
			}
			else {
				BOOST_LOG_TRIVIAL(error) << "handle_recv_message: " + ec.message();
				m_context.async_send_feedback(EError::INTERNAL_ERROR, 
							      boost::bind(&CTCPConnection::handle_write_response, 
							      shared_from_this(), placeholders::error));
			}
		}

		void handle_write_response(const boost::system::error_code& ec) {
			m_context.wflush();
			if (!ec) { 
				BOOST_LOG_TRIVIAL(info) << "Response transmitted successfully";
			}
			else {
				boost::system::error_code shutdown_error;
				if (ec.value() == boost::system::errc::broken_pipe) {
					BOOST_LOG_TRIVIAL(info) << "Disconnecting";
					m_context.close_socket(shutdown_error);
					if (shutdown_error) {
						BOOST_LOG_TRIVIAL(error) << "Unable to shutdown socket: " + shutdown_error.message();
					}

				}
				BOOST_LOG_TRIVIAL(error) << "handle_write_response: " +  ec.message();
				return;
			}

			if (!m_authorized) {
				BOOST_LOG_TRIVIAL(info) << "Disconnecting";
				boost::system::error_code shutdown_error;
				m_context.close_socket(shutdown_error);
				if (shutdown_error) {
					BOOST_LOG_TRIVIAL(error) << "Unable to shutdown socket: " + shutdown_error.message();
				}
				return;
			}
			if (m_context.socket_opened()) {
				BOOST_LOG_TRIVIAL(info) << "Reading new command...";
				m_context.async_recv_message(boost::bind(&CTCPConnection::handle_recv_message,
							     shared_from_this(), placeholders::error));
			}
		}

		CContext& context() {
			return m_context;
		}
};

#endif //CTCPCONNECTION_HPP
