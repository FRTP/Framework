#include "ccommand.h"

CCmdGetFile::CCmdGetFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetFile::CCmdGetFile()");
	}
	m_filename = *(args.begin());
	m_newfilename = *(std::next(args.begin(), 1));
	m_force_update = (*(std::next(args.begin(), 2)) == "True") ? true : false;
}

CCmdGetFile::CCmdGetFile(__attribute__((unused)) const CMessage& msg) {
	//TODO
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

EError CCmdGetFile::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>{ m_newfilename });
	if (datatype_instance == nullptr) {
		return EError::INTERNAL_ERROR;
	}
	if (!fs::exists(datatype_instance->full_path()) || m_force_update) {
		if (!fs::exists(datatype_instance->path())) {
			fs::create_directories(datatype_instance->path());
		}
		CMessage msg(ECommand::GET_FILE, datatype, std::vector<char>(m_filename.begin(), m_filename.end()));
		EError ret; ;
		if ((ret = context->send_message(msg)) != EError::OK) {
			return ret;
		}
		if ((ret = context->recv_message(msg)) != EError::OK) {
			return ret;
		}
		if ((ret = check_message(msg)) != EError::OK) {
			return ret;
		}
		return datatype_instance->write_data(msg.data_begin(), msg.data_end());
	}
	return EError::OK;
}

CCmdGetMD5::CCmdGetMD5(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetMD5::CCmdGetMD5()");
	}
	m_filename = args.front();

	m_hash = md5sum_ptr(new md5sum);
	m_hash->resize(MD5_DIGEST_LENGTH);
}

CCmdGetMD5::CCmdGetMD5(__attribute__((unused)) const CMessage& msg) {
	m_hash = md5sum_ptr(new md5sum);
	m_hash->resize(MD5_DIGEST_LENGTH);
	//TODO
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

EError CCmdGetMD5::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename });
	if (datatype_instance == nullptr) {
		return EError::INTERNAL_ERROR;
	}
	if (!fs::exists(datatype_instance->full_path())) {
		throw ExNoFile("Invalid path " + datatype_instance->full_path(), "CCmdGetMD5::invoke()");
	}

	//TODO: CMessage(ECommand, EDataType, const std::string&)
	CMessage msg(ECommand::GET_MD5, datatype, std::vector<char>(m_filename.begin(), m_filename.end()));
	EError ret;
	if ((ret = context->send_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != EError::OK) {
		return ret;
	}
	if (msg.data().size() != MD5_DIGEST_LENGTH) {
		return EError::INTERNAL_ERROR;
	}
	for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		(*m_hash)[i] = (msg.data())[i];
	}
	return EError::OK;
}

md5sum_ptr CCmdGetMD5::hash() const {
	return m_hash;
}

CCmdUploadFile::CCmdUploadFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdUploadFile::CCmdUploadFile()");
	}
	m_filename = args.front();
}

CCmdUploadFile::CCmdUploadFile(__attribute__((unused)) const CMessage& msg) {
	//TODO
}

ECommand CCmdUploadFile::type() const {
	return ECommand::UPLOAD_FILE;
}

EError CCmdUploadFile::invoke(CContext* context, EDataType datatype) {
	data_t data_buf;
	str_to_data_t(m_filename, data_buf);
	data_buf.push_back('\n');
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename }); 
	if (datatype_instance == nullptr) {
		return EError::INTERNAL_ERROR;
	}
	EError ret;
	if ((ret = datatype_instance->append_data(data_buf)) != EError::OK) {
		return ret;
	}
	CMessage msg(ECommand::UPLOAD_FILE, datatype, data_buf);
	if ((ret = context->send_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != EError::OK) {
		return ret;
	}
	else if (msg.command() != ECommand::FEEDBACK) {
		return EError::INTERNAL_ERROR;
	}
	return EError::OK;
}

CCmdAuthorize::CCmdAuthorize(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdAuthorize::CCmdAuthorize()");
	}
	m_login = args.front();
	m_password = *(std::next(args.begin(), 1));
}

CCmdAuthorize::CCmdAuthorize(__attribute__((unused)) const CMessage& msg) {
	//TODO
}

ECommand CCmdAuthorize::type() const {
	return ECommand::AUTHORIZE;
}

EError CCmdAuthorize::invoke(CContext* context, EDataType datatype) {
	data_t data_buf;
	std::string s_data = m_login + "\n";
	str_to_data_t(s_data, data_buf);
	data_buf.reserve(data_buf.size() + SHA512_DIGEST_LENGTH);
	auto sha_hash_ptr = encrypt_string(m_password);
	for (auto i : *sha_hash_ptr) {
		data_buf.push_back(i);
	}
	CMessage msg(ECommand::AUTHORIZE, datatype, data_buf);
	EError ret;
	if ((ret = context->send_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = context->recv_message(msg)) != EError::OK) {
		return ret;
	}
	if ((ret = check_message(msg)) != EError::OK) {
		return ret;
	}
	else if (msg.command() != ECommand::FEEDBACK) {
		return EError::INTERNAL_ERROR;
	}
	return EError::OK;
}
