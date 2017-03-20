#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <cassert>
#include <vector>

#include "../datatype.h"
#include "../utility.h"

class CCmdGetFile : public ICommand {
	private:
		std::string m_filename;
	public:
		explicit CCmdGetFile(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdGetFile() {}
};

class CCmdGetMD5 : public ICommand {
	private:
		std::string m_filename;
	public:
		explicit CCmdGetMD5(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdGetMD5() {}
};

class CCmdUploadFile : public ICommand {
	private:
		std::string m_filename;
		std::vector<char>::iterator m_data_begin;
		std::vector<char>::iterator m_data_end;
	public:
		explicit CCmdUploadFile(const CMessage& msg);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdUploadFile() {}
};

#endif //CCOMMAND_H
