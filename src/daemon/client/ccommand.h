#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <boost/asio.hpp>
#include <fstream>
#include <string>

#include "ccontext.hpp"

using namespace boost::asio;

enum ECommand {
	GET_FILE,
	GET_MD5
};

class ICommand {
	public:
		virtual ECommand type() const = 0;
		virtual void invoke(const std::shared_ptr<CContext>& context, int& error) = 0;
		virtual ~ICommand();
};

class CCmdGetFile : public ICommand {
	private:
		std::string m_filename;
		std::string m_newfilename;
		streambuf m_buffer;
	public:
		explicit CCmdGetFile(const std::string& filename, const std::string& newfilename);
		virtual ECommand type() const;
		virtual void invoke(const std::shared_ptr<CContext>& context, int& error);
		~CCmdGetFile() {}
};

class CCmdGetMD5 : public ICommand {
	private:
		std::string m_filename;
		const unsigned char* m_hash;
	public:
		explicit CCmdGetMD5(const std::string& filename);
		virtual ECommand type() const;
		virtual void invoke(const std::shared_ptr<CContext>& context, int& error);
		const unsigned char* hash();
		~CCmdGetMD5() {}
};

#endif //CCOMMAND_H
