#include "cclient.h"

CClient::CClient(const std::string& server, int port) {
	m_server = server;
	m_port = port;
	m_io_service = boost::shared_ptr<io_service>(new io_service());

	CCommandFactory::add<CCmdGetFile>("GetFile");
	CCommandFactory::add<CCmdGetMD5>("GetMD5");
}

std::string CClient::_get_text_error(EServerError error) {
	std::string res = "";
	switch (error) {
		case EServerError::NO_FILE:
			res = "No such file on server";
			break;
	}
	return res;
}

CContext* CClient::create_context() {
	CContext* context = new CContext(m_server, m_port, m_io_service);
	m_io_service->run();
	return context;
}

void CClient::connect(CContext* context) {
	boost::system::error_code error;
	context->connect(error);
	if(error) {
		throw ExConnectionProblem("Connection error: " + error.message(), "CClient::connect()");
	}
}


int CClient::invoke(CContext* context, ICommand* cmd) {
	if(context->socket_opened()) {
		int ret_error;
		cmd->invoke(context, ret_error);
		if(ret_error != 0) {
			std::cerr << "[EE]: Server error: "
				  + _get_text_error(static_cast<EServerError>(ret_error))
				  << std::endl;
			return ret_error;
		}
	}
	else {
		throw ExSocketProblem("Socket was unexpectedly closed", "CClient::invoke()");
	}
	return 0;
}

boost::python::list CClient::get_hash(CCmdGetMD5* cmd) {
	boost::python::list res;
	int size = 0;
	const unsigned char* md5sum = cmd->hash(size);
	for (int i = 0; i < size; ++i) {
		res.append(md5sum[i]);
	}
	return res;
}
