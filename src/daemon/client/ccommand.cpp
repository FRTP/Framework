#include "ccommand.h"

CCmdGetFile::CCmdGetFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetFile::CCmdGetFile()");
	}
	m_filename = *(args.begin());
	m_newfilename = *(std::next(args.begin(), 1));
	m_force_update = (*(std::next(args.begin(), 2)) == "True") ? true : false;
}

CCmdGetFile::CCmdGetFile(__attribute__((unused)) const utility::CMessage& msg) {
	//TODO
}

utility::ECommand CCmdGetFile::type() const {
	return utility::ECommand::GET_FILE;
}

utility::EError CCmdGetFile::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
	auto datatype_instance = datatypes::CDataTypeFactory::create(datatype, std::list<std::string>{ m_newfilename });
	if (datatype_instance == nullptr) {
		return utility::EError::INTERNAL_ERROR;
	}
	if (!fs::exists(datatype_instance->get_full_path()) || m_force_update) {
		if (!fs::exists(datatype_instance->get_path())) {
			fs::create_directories(datatype_instance->get_path());
		}
		utility::CMessage msg(utility::ECommand::GET_FILE, datatype, std::vector<char>(m_filename.begin(),
											       m_filename.end()));
		utility::EError ret;
		if ((ret = context->send_message(msg)) != utility::EError::OK) {
			return ret;
		}
		if ((ret = context->recv_message(msg)) != utility::EError::OK) {
			return ret;
		}
		if ((ret = utility::check_message(msg)) != utility::EError::OK) {
			return ret;
		}
		return datatype_instance->write_data(msg.data_begin(), msg.data_end());
	}
	return utility::EError::OK;
}

CCmdGetMD5::CCmdGetMD5(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetMD5::CCmdGetMD5()");
	}
	m_filename = args.front();

	m_hash = utility::md5sum_ptr(new utility::md5sum);
	m_hash->resize(MD5_DIGEST_LENGTH);
}

CCmdGetMD5::CCmdGetMD5(__attribute__((unused)) const utility::CMessage& msg) {
	m_hash = utility::md5sum_ptr(new utility::md5sum);
	m_hash->resize(MD5_DIGEST_LENGTH);
	//TODO
}

utility::ECommand CCmdGetMD5::type() const {
	return utility::ECommand::GET_MD5;
}

utility::EError CCmdGetMD5::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
	auto datatype_instance = datatypes::CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename });
	if (datatype_instance == nullptr) {
		return utility::EError::INTERNAL_ERROR;
	}
	if (!fs::exists(datatype_instance->get_full_path())) {
		throw ExNoFile("Invalid path " + datatype_instance->get_full_path(), "CCmdGetMD5::invoke()");
	}

	//TODO: CMessage(ECommand, EDataType, const std::string&)
	utility::CMessage msg(utility::ECommand::GET_MD5, datatype, std::vector<char>(m_filename.begin(), m_filename.end()));
	utility::EError ret;
	if ((ret = context->send_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = utility::check_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if (msg.data().size() != MD5_DIGEST_LENGTH) {
		return utility::EError::INTERNAL_ERROR;
	}
	for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		(*m_hash)[i] = (msg.data())[i];
	}
	return utility::EError::OK;
}

utility::md5sum_ptr CCmdGetMD5::hash() const {
	return m_hash;
}

CCmdUploadFile::CCmdUploadFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdUploadFile::CCmdUploadFile()");
	}
	m_filename = args.front();
}

CCmdUploadFile::CCmdUploadFile(__attribute__((unused)) const utility::CMessage& msg) {
	//TODO
}

utility::ECommand CCmdUploadFile::type() const {
	return utility::ECommand::UPLOAD_FILE;
}

utility::EError CCmdUploadFile::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
	utility::data_t data_buf;
	utility::str_to_data_t(m_filename, data_buf, false);
	data_buf.push_back('\n');
	auto datatype_instance = datatypes::CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename });
	if (datatype_instance == nullptr) {
		return utility::EError::INTERNAL_ERROR;
	}
	utility::EError ret;
	if ((ret = datatype_instance->get_data(data_buf, true)) != utility::EError::OK) {
		return ret;
	}
	utility::CMessage msg(utility::ECommand::UPLOAD_FILE, datatype, data_buf);
	if ((ret = context->send_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != utility::EError::OK) {
		return ret;
	}
	else if (msg.get_command() != utility::ECommand::FEEDBACK) {
		return utility::EError::INTERNAL_ERROR;
	}
	return utility::EError::OK;
}

CCmdAuthorize::CCmdAuthorize(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdAuthorize::CCmdAuthorize()");
	}
	m_login = args.front();
	m_password = *(std::next(args.begin(), 1));
}

CCmdAuthorize::CCmdAuthorize(__attribute__((unused)) const utility::CMessage& msg) {
	//TODO
}

utility::ECommand CCmdAuthorize::type() const {
	return utility::ECommand::AUTHORIZE;
}

utility::EError CCmdAuthorize::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
	utility::data_t data_buf;
	std::string s_data = m_login + "\n";
	utility::str_to_data_t(s_data, data_buf, false);
	data_buf.reserve(data_buf.size() + SHA512_DIGEST_LENGTH);
	auto sha_hash_ptr = utility::encrypt_string(m_password);
	for (auto i : *sha_hash_ptr) {
		data_buf.push_back(i);
	}
	utility::CMessage msg(utility::ECommand::AUTHORIZE, datatype, data_buf);
	utility::EError ret;
	if ((ret = context->send_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != utility::EError::OK) {
		return ret;
	}
	else if (msg.get_command() != utility::ECommand::FEEDBACK) {
		return utility::EError::INTERNAL_ERROR;
	}
	return utility::EError::OK;
}

CCmdRegister::CCmdRegister(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdRegister::CCmdRegister()");
	}
	m_login = args.front();
	m_password = *(std::next(args.begin(), 1));
}

CCmdRegister::CCmdRegister(__attribute__((unused)) const utility::CMessage& msg) {
	//TODO
}

utility::ECommand CCmdRegister::type() const {
	return utility::ECommand::REGISTER;
}

utility::EError CCmdRegister::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
	utility::data_t data_buf;
	std::string s_data = m_login + "\n";
	utility::str_to_data_t(s_data, data_buf, false);
	data_buf.reserve(data_buf.size() + SHA512_DIGEST_LENGTH);
	auto sha_hash_ptr = utility::encrypt_string(m_password);
	for (auto i : *sha_hash_ptr) {
		data_buf.push_back(i);
	}
	utility::CMessage msg(utility::ECommand::REGISTER, datatype, data_buf);
	utility::EError ret;
	if ((ret = context->send_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != utility::EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != utility::EError::OK) {
		return ret;
	}
	else if (msg.get_command() != utility::ECommand::FEEDBACK) {
		return utility::EError::INTERNAL_ERROR;
	}
	return utility::EError::OK;
}
