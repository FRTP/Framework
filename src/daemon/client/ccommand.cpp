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
	if(!fs::exists(datatype_instance->full_path()) || m_force_update) {
		fs::create_directories(datatype_instance->full_path());
		CMessage msg(ECommand::GET_FILE, datatype, std::vector<char>(m_filename.begin(), m_filename.end()));
		EError ret = context->send_message(msg);
		if (ret != EError::OK) {
			return ret;
		}

		ret = context->recv_message(msg);
		if (ret != EError::OK) {
			return ret;
		}
		if (msg.command() == ECommand::FEEDBACK && msg.datatype() == EDataType::ERROR_CODE) {
			int i_error_code = static_cast<int>((msg.data())[0]);
			if (i_error_code < 0 || i_error_code >= static_cast<int>(EError::MAX_VAL)) {
				return EError::UNKNOWN_ERROR;
			}
			return static_cast<EError>(i_error_code);
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
}

CCmdGetMD5::CCmdGetMD5(__attribute__((unused)) const CMessage& msg) {
	//TODO
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

EError CCmdGetMD5::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename });
	if (!fs::exists(datatype_instance->full_path())) {
		throw ExNoFile("Invalid path " + datatype_instance->full_path(), "CCmdGetMD5::invoke()");
	}

	CMessage msg(ECommand::GET_MD5, datatype, std::vector<char>(m_filename.begin(), m_filename.end()));
	EError ret = context->send_message(msg);
	if (ret != EError::OK) {
		return ret;
	}

	ret = context->recv_message(msg);
	if (ret != EError::OK) {
		return ret;
	}
	if (msg.command() == ECommand::FEEDBACK && msg.datatype() == EDataType::ERROR_CODE) {
		int i_error_code = static_cast<int>((msg.data())[0]);
		if (i_error_code < 0 || i_error_code >= static_cast<int>(EError::MAX_VAL)) {
			return EError::UNKNOWN_ERROR;
		}
		return static_cast<EError>(i_error_code);
	}

	for (size_t i = 0; i < msg.data().size(); ++i) {
		(*m_hash)[i] = static_cast<unsigned char>((msg.data())[i]);
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
	std::vector<char> data_buf(m_filename.begin(), m_filename.end());
	data_buf.push_back('\0');
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>{ m_filename }); 
	EError ret = datatype_instance->append_data(data_buf);
	if (ret != EError::OK) {
		return ret;
	}
	CMessage msg(ECommand::UPLOAD_FILE, datatype, data_buf);
	ret = context->send_message(msg);	
	if (ret != EError::OK) {
		return ret;
	}

	ret = context->recv_message(msg);
	if (ret != EError::OK) {
		return ret;
	}
	if (msg.command() == ECommand::FEEDBACK && msg.datatype() == EDataType::ERROR_CODE) {
		int i_error_code = static_cast<int>((msg.data())[0]);
		if (i_error_code < 0 || i_error_code >= static_cast<int>(EError::MAX_VAL)) {
			return EError::UNKNOWN_ERROR;
		}
		return static_cast<EError>(i_error_code);
	}
	else {
		return EError::INTERNAL_ERROR;
	}
}
