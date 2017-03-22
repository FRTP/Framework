#include "cclient.h"

CClient::CClient(const std::string& working_dir) {
	m_io_service = boost::shared_ptr<io_service>(new io_service());

	CCommandFactory::add<CCmdGetFile>("GetFile");
	CCommandFactory::add<CCmdGetMD5>("GetMD5");
	CCommandFactory::add<CCmdUploadFile>("UploadFile");

	CDataTypeFactory::register_type<CDataTypeShares>(EDataType::SHARES);
	CDataTypeFactory::register_type<CDataTypeTwitter>(EDataType::TWITTER);

	CSettings::set_working_dir(working_dir);
}

CContext* CClient::create_context() {
	CContext* context = new CContext(*m_io_service);
	m_io_service->run();
	return context;
}

void CClient::connect(CContext* context, const std::string& server, int port) {
	boost::system::error_code error;
	context->socket().connect(ip::tcp::endpoint(ip::address::from_string(server.c_str()), port), error);
	if (error) {
		throw ExConnectionProblem("Connection error: " + error.message(), "CClient::connect()");
	}
}

void CClient::invoke(CContext* context, ICommand* cmd, int datatype) {
	if (context->socket_opened()) {
		if (datatype < 0 || datatype > static_cast<int>(EError::MAX_VAL)) {
			throw ExUnknownDataType("Invalid data type", "CClient::invoke()");
		}

		EError ret;
		if ((ret = cmd->invoke(context, static_cast<EDataType>(datatype))) != EError::OK) {
			switch (ret) {
				case EError::OPEN_ERROR:
					throw ExNoFile("No such file on server", "CClient::invoke()");
				default:
					throw ExError(get_text_error(ret), "CClient::invoke()");
				//TODO
			}
		}
	}
	else {
		throw ExSocketProblem("Socket was unexpectedly closed", "CClient::invoke()");
	}
}

std::string CClient::get_hash(CCmdGetMD5* cmd) {
	std::string res = md5sum_to_str(cmd->hash());
	return res;
}

bool CClient::check_integrity(CContext* context, const std::string& srv_filename,
			      const std::string& cli_filename, int datatype) {
	boost::shared_ptr<CCmdGetMD5> cmd = boost::shared_ptr<CCmdGetMD5>(new CCmdGetMD5(std::list<std::string>({ srv_filename })));
	invoke(context, cmd.get(), datatype);
	md5sum_ptr srv_md5_hash = cmd->hash();

	std::string full_path(CSettings::working_dir() + get_data_type_dir(static_cast<EDataType>(datatype)) + "/" + cli_filename);
	md5sum_ptr cli_md5_hash = calculate_md5(full_path);
	for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		if ((*srv_md5_hash)[i] != (*cli_md5_hash)[i]) {
			return false;
		}
	}
	return true;
}
