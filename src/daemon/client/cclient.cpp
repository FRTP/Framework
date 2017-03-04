#include "cclient.h"

CClient::CClient(io_service& io_service, char* server, int port) {
	m_context = std::shared_ptr<CContext>(new CContext(std::string(server), port, io_service));
	boost::system::error_code error;
	m_context->connect(error);
	if (error) {
		std::cerr << "Unable to connect" << std::endl;
	}
	else {
		std::cout << "Connected" << std::endl;
	}
}

std::string CClient::_get_text_error(EServerError error) const {
	std::string res = "";
	switch (error) {
		case EServerError::NO_FILE:
			res = "No such file on server";
			break;
	}
	return res;
}

void CClient::invoke(ICommand* cmd) {
	if (m_context->socket_opened()) {
		int ret_error;
		cmd->invoke(m_context, ret_error);
		if(ret_error != 0) {
			std::cerr << "[EE]: Server error: " + _get_text_error(static_cast<EServerError>(ret_error))
				  << std::endl;
		}
	}
	else {
		std::cerr << "Socket is closed" << std::endl;
	}
}
