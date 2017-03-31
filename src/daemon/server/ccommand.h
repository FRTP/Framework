#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <boost/regex.hpp>
#include <cassert>
#include <sqlite3.h>
#include <vector>

#include "../ccontext.hpp"
#include "../datatype.h"
#include "../utility.h"

using namespace datatypes;

namespace server_command {
	int auth_callback(void* cmd, int col_num, char** fields, char** columns);
	bool is_valid_login(const std::string& login);

	class IServerCommand : public ICommand {
		public:
			virtual void set_callback(CContext::callback_type callback) = 0;
			virtual ~IServerCommand() {}
	};

	class CCmdGetFile : public IServerCommand {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 2;

			std::string m_filename;
			std::string m_source;
			CContext::callback_type m_callback;
		public:
			explicit CCmdGetFile(const CMessage& msg);
			explicit CCmdGetFile(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type  callback);
			~CCmdGetFile() {}
	};

	class CCmdGetMD5 : public IServerCommand {
		private:
			std::string m_filename;
			CContext::callback_type m_callback;
		public:
			explicit CCmdGetMD5(const CMessage& msg);
			explicit CCmdGetMD5(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type callback);
			~CCmdGetMD5() {}
	};

	class CCmdUploadFile : public IServerCommand {
		private:
			std::string m_filename;
			data_t::const_iterator m_data_begin;
			data_t::const_iterator m_data_end;
			CContext::callback_type m_callback;
		public:
			explicit CCmdUploadFile(const CMessage& msg);
			explicit CCmdUploadFile(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type callback);
			~CCmdUploadFile() {}
	};

	class CCmdAuthorize : public IServerCommand {
		private:
			std::string m_login;
			std::string m_password;
			bool m_authorized;
			CContext::callback_type m_callback;
		public:
			explicit CCmdAuthorize(const CMessage& msg);
			explicit CCmdAuthorize(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type callback);
			const std::string& login() const;
			const std::string& password() const;
			void make_authorized();
			~CCmdAuthorize() {}
	};

	class CCmdRegister : public IServerCommand {
		private:
			std::string m_login;
			std::string m_password;
			CContext::callback_type m_callback;
		public:
			explicit CCmdRegister(const CMessage& msg);
			explicit CCmdRegister(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type callback);
	};
}

#endif //CCOMMAND_H
