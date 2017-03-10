#include "cclient.h"

CClient::CClient(const std::string& server, int port, const std::string& working_dir) {
	m_server = server;
	m_port = port;
	m_io_service = boost::shared_ptr<io_service>(new io_service());

	CCommandFactory::add<CCmdGetFile>("GetFile");
	CCommandFactory::add<CCmdGetMD5>("GetMD5");
	CCommandFactory::add<CCmdUploadFile>("UploadFile");

	CSettings::set_working_dir(working_dir);
}

CContext* CClient::create_context() {
	CContext* context = new CContext(m_server, m_port, m_io_service);
	m_io_service->run();
	return context;
}

void CClient::connect(CContext* context) {
	boost::system::error_code error;
	context->connect(error);
	if (error) {
		throw ExConnectionProblem("Connection error: " + error.message(), "CClient::connect()");
	}
}

int CClient::invoke(CContext* context, ICommand* cmd, int datatype) {
	if (context->socket_opened()) {
		if (datatype < 0 || datatype > static_cast<EError::MAX_VAL>) {
			throw ExUnknownDataType("Invalid data type", "CClient::invoke()");
		}

		EError ret;
		if ((ret = cmd->invoke(context, static_cast<EDataType>(datatype))) != EError::OK) {
			std::cerr << "[EE]: Server error: " << get_text_error(ret) << std::endl;
			return static_cast<int>(ret);
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

bool CClient::check_integrity(CContext* context, const std::string& srv_filename,
			      const std::string& cli_filename, int datatype) {
	bool res = true;
	CCmdGetMD5* cmd = new CCmdGetMD5(std::list<std::string>({ srv_filename }));
	invoke(context, cmd, datatype);
	int srv_hash_size = 0;
	auto srv_md5_hash = cmd->hash(srv_hash_size);
	delete cmd;

	int cli_hash_size = 0;
	std::string full_path(CSettings::working_dir() + get_data_type_dir() +
			      "/" + cli_filename);
	auto cli_md5_hash = calculate_md5(full_path, cli_hash_size);
	if (srv_hash_size != cli_hash_size) {
		return false;
	}
	for (int i = 0; i < srv_hash_size; ++i) {
		if (srv_md5_hash[i] != cli_md5_hash[i]) {
			res = false;
			break;
		}
	}
	delete cli_md5_hash;

	return res;
}
