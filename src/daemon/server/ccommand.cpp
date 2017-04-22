#include "ccommand.h"

namespace server_command {
	CCmdGetFile::CCmdGetFile(const utility::CMessage& msg) {
		m_filename = std::string(reinterpret_cast<const char*>(&(msg.data())[0]), msg.data().size());
	}

	CCmdGetFile::CCmdGetFile(__attribute__ ((unused)) const std::list<std::string>& args) {
		//TODO
	}

	utility::ECommand CCmdGetFile::type() const {
		return utility::ECommand::GET_FILE;
	}

	utility::EError CCmdGetFile::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
		auto datatype_instance = datatypes::CDataTypeFactory::create(
				datatype,
				std::list<std::string>({ m_filename })
				);
		if (!datatype_instance->is_success()) {
			return utility::EError::INTERNAL_ERROR;
		}
		utility::data_t data_buf;
		utility::EError ret;
		if ((ret = datatype_instance->get_data(data_buf)) != utility::EError::OK) {
			return ret;
		}

		utility::CMessage msg_file(utility::ECommand::GET_FILE, datatype, data_buf);
		context->async_send_message(msg_file, m_callback);

		return utility::EError::OK;
	}

	void CCmdGetFile::set_callback(CContext::callback_type callback) {
		m_callback = callback;
	}

	CCmdGetMD5::CCmdGetMD5(const utility::CMessage& msg) {
		m_filename = std::string(reinterpret_cast<const char*>(&(msg.data())[0]), msg.data().size());
	}

	CCmdGetMD5::CCmdGetMD5(__attribute__ ((unused)) const std::list<std::string>& args) {
		//TODO
	}

	utility::ECommand CCmdGetMD5::type() const {
		return utility::ECommand::GET_MD5;
	}

	utility::EError CCmdGetMD5::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
		if (datatype == utility::EDataType::ASSETS) {
			utility::md5sum_ptr md5 = utility::calculate_md5(utility::get_full_path(datatype,
												m_filename));
			if (md5 == nullptr) {
				return utility::EError::INTERNAL_ERROR;
			}
			utility::CMessage msg_md5(utility::ECommand::GET_MD5, datatype, *md5);
			context->async_send_message(msg_md5, m_callback);

			return utility::EError::OK;
		}
		else {
			//TODO
			return utility::EError::OK;
		}
	}

	void CCmdGetMD5::set_callback(CContext::callback_type callback) {
		m_callback = callback;
	}

	CCmdUploadFile::CCmdUploadFile(const utility::CMessage& msg) {
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

	utility::ECommand CCmdUploadFile::type() const {
		return utility::ECommand::UPLOAD_FILE;
	}

	utility::EError CCmdUploadFile::invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype) {
		auto datatype_instance = datatypes::CDataTypeFactory::create(datatype, std::list<std::string>({ m_filename }));
		if (!datatype_instance->is_success()) {
			return utility::EError::INTERNAL_ERROR;
		}
		datatype_instance->write_data(m_data_begin, m_data_end);
		context->async_send_feedback(utility::EError::OK, m_callback);

		return utility::EError::OK;
	}

	void CCmdUploadFile::set_callback(CContext::callback_type callback) {
		m_callback = callback;
	}

	CCmdAuthorize::CCmdAuthorize(const utility::CMessage& msg)
		: m_authorized(false) {
		auto it = msg.data_begin();

		while (*it != '\n') {
			++it;
		}

		m_login = std::string(msg.data_begin(), it);
		utility::sha512_ptr pwd_hash_ptr(new utility::sha512(++it, msg.data_end()));
		m_password = utility::hash_to_str(pwd_hash_ptr);
	}

	CCmdAuthorize::CCmdAuthorize(__attribute__ ((unused)) const std::list<std::string>& args)
		: m_authorized(false) {
		//TODO
	}

	utility::ECommand CCmdAuthorize::type() const {
		return utility::ECommand::AUTHORIZE;
	}

	utility::EError CCmdAuthorize::invoke(__attribute__ ((unused)) boost::shared_ptr<CContext>& context,
					      __attribute__ ((unused)) utility::EDataType datatype) {
		if (!is_valid_login(m_login)) {
			return utility::EError::AUTH_ERROR;
		}

		sqlite3* db = 0;
		std::string query = "select password from users where login = '" + m_login + "';";
		char* err = 0;

		if (sqlite3_open((utility::CSettings::get_working_dir() + "db/users.db").c_str(), &db)) {
			return utility::EError::DB_ERROR;
		}

		sqlite3_exec(db, query.c_str(), auth_callback, reinterpret_cast<void*>(this), &err);
		sqlite3_free(err);
		sqlite3_close(db);
		utility::EError ret = m_authorized ? utility::EError::OK : utility::EError::AUTH_ERROR;
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

	CCmdRegister::CCmdRegister(const utility::CMessage& msg) {
		auto it = msg.data_begin();

		while (*it != '\n') {
			++it;
		}

		m_login = std::string(msg.data_begin(), it);
		utility::sha512_ptr pwd_hash_ptr(new utility::sha512(++it, msg.data_end()));
		m_password = utility::hash_to_str(pwd_hash_ptr);
	}

	CCmdRegister::CCmdRegister(__attribute__ ((unused)) const std::list<std::string>& args) {
		//TODO
	}

	utility::ECommand CCmdRegister::type() const {
		return utility::ECommand::REGISTER;
	}

	utility::EError CCmdRegister::invoke(boost::shared_ptr<CContext>& context,
					     __attribute__ ((unused)) utility::EDataType datatype) {
		if (!is_valid_login(m_login)) {
			return utility::EError::INVALID_LOGIN;
		}

		sqlite3* db = 0;
		std::string query = "insert into users (login,password)  values ('" + m_login + "','" + m_password + "');";
		char* err = 0;

		if (sqlite3_open((utility::CSettings::get_working_dir() + "db/users.db").c_str(), &db)) {
			return utility::EError::DB_ERROR;
		}

		if (sqlite3_exec(db, query.c_str(), 0, 0, &err) != SQLITE_OK) {
			sqlite3_free(err);
			sqlite3_close(db);
			return utility::EError::INVALID_LOGIN;
		}
		sqlite3_free(err);
		sqlite3_close(db);
		context->async_send_feedback(utility::EError::OK, m_callback);
		return utility::EError::OK;
	}

	void CCmdRegister::set_callback(CContext::callback_type callback) {
		m_callback = callback;
	}

	bool is_valid_login(const std::string& login) {
		boost::regex login_regex("[A-Z,a-z,0-9]+");
		boost::smatch result;
		return boost::regex_match(login, result, login_regex);
	}

	int auth_callback(void* cmd,
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
}
