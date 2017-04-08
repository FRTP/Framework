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

#include "../ccontext.h"
#include "datatype.h"
#include "exception.hpp"
#include "../utility.h"

namespace fs = boost::filesystem;

class CCmdGetFile : public utility::ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 3;
		std::string m_filename;
		std::string m_newfilename;
		boost::asio::streambuf m_buffer;
		bool m_force_update;
	public:
		explicit CCmdGetFile(const std::list<std::string>& args);
		explicit CCmdGetFile(const utility::CMessage& msg);
		virtual utility::ECommand type() const;
		virtual utility::EError invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype);
		virtual ~CCmdGetFile() {}
};

class CCmdGetMD5 : public utility::ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
		utility::md5sum_ptr m_hash;
	public:
		explicit CCmdGetMD5(const std::list<std::string>& args);
		explicit CCmdGetMD5(const utility::CMessage& msg);
		virtual utility::ECommand type() const;
		virtual utility::EError invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype);
		utility::md5sum_ptr hash() const;
		virtual ~CCmdGetMD5() {}
};

class CCmdUploadFile : public utility::ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
	public:
		explicit CCmdUploadFile(const std::list<std::string>& args);
		explicit CCmdUploadFile(const utility::CMessage& msg);
		virtual utility::ECommand type() const;
		virtual utility::EError invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype);
		virtual ~CCmdUploadFile() {}
};

class CCmdAuthorize : public utility::ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 2;
		std::string m_login;
		std::string m_password;
	public:
		explicit CCmdAuthorize(const std::list<std::string>& args);
		explicit CCmdAuthorize(const utility::CMessage& msg);
		virtual utility::ECommand type() const;
		virtual utility::EError invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype);
		virtual ~CCmdAuthorize() {}
};

class CCmdRegister : public utility::ICommand {
	private:
		static constexpr int EXPECTED_ARGS_NUM = 2;
		std::string m_login;
		std::string m_password;
	public:
		explicit CCmdRegister(const std::list<std::string>& args);
		explicit CCmdRegister(const utility::CMessage& msg);
		virtual utility::ECommand type() const;
		virtual utility::EError invoke(boost::shared_ptr<CContext>& context, utility::EDataType datatype);
		virtual ~CCmdRegister() {}
};

#endif //CCOMMAND_H
