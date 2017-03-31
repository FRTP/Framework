#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <openssl/md5.h>

#include "ccommand.h"
#include "../ccontext.hpp"
#include "exception.hpp"

using namespace boost::asio;
using namespace datatypes;
using namespace utility;

class CClient {
	private:
		std::string m_working_dir;
		boost::shared_ptr<io_service> m_io_service;
	public:
		CClient(const std::string& working_dir);
		~CClient() {}
		CContext* create_context();
		static void connect(CContext* context, const std::string& server, int port,
				    const std::string& login, const std::string& password);
		static void invoke(CContext* context, ICommand* cmd, int datatype);
		static std::string get_hash(CCmdGetMD5* cmd);
		static bool check_integrity(CContext* context, const std::string& srv_filename, 
					    const std::string& cli_filename, int datatype);
};

#endif //CLIENT_H
