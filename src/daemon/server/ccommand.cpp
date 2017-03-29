#include "ccommand.h"

using namespace server_command;

CCmdGetFile::CCmdGetFile(const CMessage& msg) {
	m_filename = std::string(reinterpret_cast<const char*>(&(msg.data())[0]), msg.data().size());
}

CCmdGetFile::CCmdGetFile(__attribute__ ((unused)) const std::list<std::string>& args) {
	//TODO
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

EError CCmdGetFile::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ m_filename }));
	if (!datatype_instance->success()) {
		return EError::INTERNAL_ERROR;
	}
	data_t data_buf;
	EError ret;
	if ((ret = datatype_instance->get_data(data_buf)) != EError::OK) {
		return ret;
	}

	CMessage msg_file(ECommand::GET_FILE, datatype, data_buf);
	context->async_send_message(msg_file, m_callback);

	return EError::OK;
}

void CCmdGetFile::set_callback(CContext::callback_type callback) {
	m_callback = callback;
}

CCmdGetMD5::CCmdGetMD5(const CMessage& msg) {
	m_filename = std::string(reinterpret_cast<const char*>(&(msg.data())[0]), msg.data().size());
}

CCmdGetMD5::CCmdGetMD5(__attribute__ ((unused)) const std::list<std::string>& args) {
	//TODO
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

EError CCmdGetMD5::invoke(CContext* context, EDataType datatype) {
	md5sum_ptr md5 = calculate_md5(get_full_path(datatype, m_filename));
	if (md5 == nullptr) {
		return EError::INTERNAL_ERROR;
	}
	CMessage msg_md5(ECommand::GET_MD5, datatype, *md5);
	context->async_send_message(msg_md5, m_callback);

	return EError::OK;
}

void CCmdGetMD5::set_callback(CContext::callback_type callback) {
	m_callback = callback;
}

CCmdUploadFile::CCmdUploadFile(const CMessage& msg) {
	auto it = msg.data_begin();

	//determine the end of file name (where the file content begins)
	while (*it != '\n') {
		++it;
	}
	m_filename = std::string(msg.data_begin(), it);

	m_data_begin = ++it;
	m_data_end = msg.data_end();
}

CCmdUploadFile::CCmdUploadFile(__attribute__ ((unused)) const std::list<std::string>& args) {
	//TODO
}

ECommand CCmdUploadFile::type() const {
	return ECommand::UPLOAD_FILE;
}

EError CCmdUploadFile::invoke(CContext* context, EDataType datatype) {
	auto datatype_instance = CDataTypeFactory::create(datatype, std::list<std::string>({ m_filename }));
	if (!datatype_instance->success()) {
		return EError::INTERNAL_ERROR;
	}
	datatype_instance->write_data(m_data_begin, m_data_end);
	context->async_send_feedback(EError::OK, m_callback);

	return EError::OK;
}

void CCmdUploadFile::set_callback(CContext::callback_type callback) {
	m_callback = callback;
}
