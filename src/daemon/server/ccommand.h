#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <cassert>
#include <vector>

#include "../ccontext.hpp"
#include "../datatype.h"
#include "../utility.h"

using namespace datatypes;

namespace server_command {
	class IServerCommand : public ICommand {
		public:
			virtual void set_callback(CContext::callback_type callback) = 0;
			virtual ~IServerCommand() {}
	};

	class CCmdGetFile : public IServerCommand {
		private:
			std::string m_filename;
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
			std::vector<char>::const_iterator m_data_begin;
			std::vector<char>::const_iterator m_data_end;
			CContext::callback_type m_callback;
		public:
			explicit CCmdUploadFile(const CMessage& msg);
			explicit CCmdUploadFile(const std::list<std::string>& args);
			virtual ECommand type() const;
			virtual EError invoke(CContext* context, EDataType datatype);
			virtual void set_callback(CContext::callback_type callback);
			~CCmdUploadFile() {}
	};
}

#endif //CCOMMAND_H
