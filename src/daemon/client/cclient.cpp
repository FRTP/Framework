#include "cclient.h"

CClient::CClient(const std::string& working_dir, const std::string& data_subdir) {
	m_io_service = boost::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());

	utility::CCommandFactory::add<CCmdGetFile>("GetFile");
	utility::CCommandFactory::add<CCmdGetMD5>("GetMD5");
	utility::CCommandFactory::add<CCmdUploadFile>("UploadFile");
	utility::CCommandFactory::add<CCmdAuthorize>("Authorize");
	utility::CCommandFactory::add<CCmdRegister>("Register");

	datatypes::CDataTypeFactory::register_type<datatypes::CDataTypeAssets>(utility::EDataType::ASSETS);
	datatypes::CDataTypeFactory::register_type<datatypes::CDataTypeTwitter>(utility::EDataType::TWITTER);

	utility::CSettings::set_working_dir(working_dir);
	utility::CSettings::set_data_dir(data_subdir);
}

void CClient::connect(const std::string& server, int port, const std::string& login,
		      const std::string& password) {
	m_context = boost::shared_ptr<CContext>(new CContext(*m_io_service));
	m_io_service->run();

	boost::system::error_code error;
	m_context->socket().connect(boost::asio::ip::tcp::endpoint(
				    boost::asio::ip::address::from_string(server.c_str()), port),
				    error
				    );
	if (error) {
		throw ExConnectionProblem("Connection error: " + error.message(), "CClient::connect()");
	}

	boost::python::list args;
	args.append(login);
	args.append(password);
	auto cmd = utility::CCommandFactory::create("Authorize", args);
	invoke(cmd, static_cast<int>(utility::EDataType::ACCOUNT));
}

void CClient::invoke(utility::ICommand* cmd, int datatype) {
	if (m_context->socket_opened()) {
		if (datatype < 0 || datatype > static_cast<int>(utility::EError::MAX_VAL)) {
			throw ExUnknownDataType("Invalid data type", "CClient::invoke()");
		}

		utility::EError ret;
		if ((ret = cmd->invoke(m_context, static_cast<utility::EDataType>(datatype)))
				!= utility::EError::OK) {
			switch (ret) {
				case utility::EError::OPEN_ERROR:
					throw ExNoFile("Invalid file name", "CClient::invoke()");
				default:
					throw ExError(utility::get_text_error(ret), "CClient::invoke()");
				//TODO
			}
		}
	}
	else {
		throw ExSocketProblem("Socket was unexpectedly closed", "CClient::invoke()");
	}
}

std::string CClient::get_hash(CCmdGetMD5* cmd) {
	std::string res = utility::hash_to_str(cmd->hash());
	return res;
}

bool CClient::check_integrity(const std::string& srv_filename,
			      const std::string& cli_filename, int datatype) {
	auto cmd = boost::shared_ptr<CCmdGetMD5>(new CCmdGetMD5(std::list<std::string>{ srv_filename }));
	if (cmd == nullptr) {
		throw ExError("Unable to create GetMD5 command", "CClient::check_integrity()");
	}
	invoke(cmd.get(), datatype);
	utility::md5sum_ptr srv_md5_hash = cmd->hash();
	if (srv_md5_hash == nullptr) {
		throw ExError("Unable to correctly obtain MD5 from server", "CClient::check_integrity()");
	}

	std::string full_path = utility::get_full_path(static_cast<utility::EDataType>(datatype), cli_filename);
	utility::md5sum_ptr cli_md5_hash = utility::calculate_md5(full_path);
	if (cli_md5_hash == nullptr) {
		throw ExError("Unable to calculate MD5", "CClient::check_integrity()");
	}

	if (srv_md5_hash->size() != MD5_DIGEST_LENGTH ||
	    cli_md5_hash->size() != MD5_DIGEST_LENGTH) {
		return false;
	}
	for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		if ((*srv_md5_hash)[i] != (*cli_md5_hash)[i]) {
			return false;
		}
	}
	return true;
}
