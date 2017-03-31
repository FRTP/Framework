#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/python.hpp>
#include <fstream>
#include <map>
#include <list>
#include <string>
#include <vector>

#include "../ccontext.hpp"
#include "../datatype.h"
#include "exception.hpp"
#include "../utility.h"

namespace fs = boost::filesystem;
using namespace boost::asio;
using namespace datatypes;
using namespace utility;

class CCmdGetFile : public ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 3;
		std::string m_filename;
		std::string m_newfilename;
		streambuf m_buffer;
		bool m_force_update;
	public:
		explicit CCmdGetFile(const std::list<std::string>& args);
		explicit CCmdGetFile(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdGetFile() {}
};

class CCmdGetMD5 : public ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
		md5sum_ptr m_hash;
	public:
		explicit CCmdGetMD5(const std::list<std::string>& args);
		explicit CCmdGetMD5(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		md5sum_ptr hash() const;
		~CCmdGetMD5() {}
};

class CCmdUploadFile : public ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
	public:
		explicit CCmdUploadFile(const std::list<std::string>& args);
		explicit CCmdUploadFile(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdUploadFile() {}
};

class CCmdAuthorize : public ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 2;
		std::string m_login;
		std::string m_password;
	public:
		explicit CCmdAuthorize(const std::list<std::string>& args);
		explicit CCmdAuthorize(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdAuthorize() {}
};

class CCmdRegister : public ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 2;
		std::string m_login;
		std::string m_password;
	public:
		explicit CCmdRegister(const std::list<std::string>& args);
		explicit CCmdRegister(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdRegister() {}
};

#endif //CCOMMAND_H
