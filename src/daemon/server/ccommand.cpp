#include "ccommand.h"

CCmdGetFile::CCmdGetFile(const CMessage& msg) {
	m_filename = std::string(&(msg.data())[0], msg.data().size());
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

EError CCmdGetFile::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ m_filename }));
	if (!datatype_instance->success()) {
		if (datatype_instance)
			delete datatype_instance;
		}
		return EError::INTERNAL_ERROR;
	}
	std::vector<char> data_buf;
	if ((ret = datatype_instance->get_data(data_buf)) != EError::OK) {
		if (datatype_instance) {
			delete datatype_instance;
		}
		return ret;
	}
	if (datatype_instance) {
		delete datatype_instance;
	}

	CMessage msg_file(ECommand::GET_FILE, datatype, data_buf);
	context->async_send_message(msg_file);

	return EError::OK;
}

CCmdGetMD5::CCmdGetMD5(const CMessage& msg) {
	m_filename = std::string(&(msg.data())[0], msg.data().size());
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

EError CCmdGetMD5::invoke(CContext* context, EDataType datatype) {
	md5sum_ptr md5 = calculate_md5(m_filename);
	std::vector<char> data_buf;
	data_buf.resize(MD5_DIGEST_LENGTH);
	for (auto i : *md5) {
		data_buf[i] = static_cast<char>(i);
	}
	CMessage msg_md5(ECommand::GET_MD5, datatype, data_buf);
	context->async_send_message(msg_md5);

	return EError::OK;
}

CCmdUploadFile::CCmdUploadFile(const CMessage& msg) {
	auto it = msg.data_begin();

	//determine the end of file name (where the file content begins)
	while (it != '\0') {
		++it;
	}
	m_data_begin = ++it;
	m_data_end = msg.data_end();
	
	m_filename = std::string(msg.data_begin(), it);
}

ECommand CCmdUploadFile::type() const {
	return ECommand::UPLOAD_FILE;
}

EError CCmdUploadFile::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ m_filename }));
	if (!datatype_instance->success()) {
		if (datatype_instance) {
			delete datatype_instance;
		}
		return EError::INTERNAL_ERROR;
	}
	datatype_instance->write_data(m_data_begin, m_data_end);
	if (datatype_instance) {
		delete datatype_instance;
	}
	context->async_send_feedback(EError::OK);

	return EError::OK;
}
