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

CCmdAuthorize::CCmdAuthorize(const CMessage& msg)
	: m_authorized(false) {
	auto it = msg.data_begin();

	while (*it != '\n') {
		++it;
	}

	m_login = std::string(msg.data_begin(), it);
	sha512_ptr pwd_hash_ptr(new sha512(++it, msg.data_end()));
	m_password = hash_to_str(pwd_hash_ptr);
}

CCmdAuthorize::CCmdAuthorize(__attribute__ ((unused)) const std::list<std::string>& args)
	: m_authorized(false) {
	//TODO
}

ECommand CCmdAuthorize::type() const {
	return ECommand::AUTHORIZE;
}

EError CCmdAuthorize::invoke(__attribute__ ((unused)) CContext* context, __attribute__ ((unused)) EDataType datatype) {
	if (!is_valid_login(m_login)) {
		return EError::AUTH_ERROR;
	}

	sqlite3* db = 0;
	std::string query = "select password from users where login = '" + m_login + "';";
	char* err = 0;

	if (sqlite3_open((CSettings::working_dir() + "db/users.db").c_str(), &db)) {
		return EError::DB_ERROR;
	}

	sqlite3_exec(db, query.c_str(), auth_callback, reinterpret_cast<void*>(this), &err);
	sqlite3_free(err);
	sqlite3_close(db);
	EError ret = m_authorized ? EError::OK : EError::AUTH_ERROR;
	return ret;
}

void CCmdAuthorize::set_callback(CContext::callback_type callback) {
	m_callback = callback;
}

const std::string& CCmdAuthorize::login() const {
	return m_login;
}

const std::string& CCmdAuthorize::password() const {
	return m_password;
}

void CCmdAuthorize::make_authorized() {
	m_authorized = true;
}

CCmdRegister::CCmdRegister(const CMessage& msg) {
	auto it = msg.data_begin();

	while (*it != '\n') {
		++it;
	}

	m_login = std::string(msg.data_begin(), it);
	sha512_ptr pwd_hash_ptr(new sha512(++it, msg.data_end()));
	m_password = hash_to_str(pwd_hash_ptr);
}

CCmdRegister::CCmdRegister(__attribute__ ((unused)) const std::list<std::string>& args) {
	//TODO
}

ECommand CCmdRegister::type() const {
	return ECommand::REGISTER;
}

EError CCmdRegister::invoke(CContext* context, __attribute__ ((unused)) EDataType datatype) {
	if (!is_valid_login(m_login)) {
		return EError::INVALID_LOGIN;
	}

	sqlite3* db = 0;
	std::string query = "insert into users (login,password)  values ('" + m_login + "','" + m_password + "');";
	char* err = 0;

	if (sqlite3_open((CSettings::working_dir() + "db/users.db").c_str(), &db)) {
		return EError::DB_ERROR;
	}

	if (sqlite3_exec(db, query.c_str(), 0, 0, &err) != SQLITE_OK) {
		sqlite3_free(err);
		sqlite3_close(db);
		return EError::INVALID_LOGIN;
	}
	sqlite3_free(err);
	sqlite3_close(db);
	context->async_send_feedback(EError::OK, m_callback);
	return EError::OK;
}

void CCmdRegister::set_callback(CContext::callback_type callback) {
	m_callback = callback;
}

bool server_command::is_valid_login(const std::string& login) {
	boost::regex login_regex("[A-Z,a-z,0-9]+");
	boost::smatch result;
	return boost::regex_match(login, result, login_regex);
}

int server_command::auth_callback(void* cmd,
				  __attribute__ ((unused)) int col_num,
				  char** fields,
				  __attribute__ ((unused)) char** columns) {
	auto cmd_instance = reinterpret_cast<CCmdAuthorize*>(cmd);
	std::string db_pwd(fields[0] ? fields[0] : "NULL");
	if (db_pwd == cmd_instance->password()) {
		cmd_instance->make_authorized();
	}
	return 0;
}
