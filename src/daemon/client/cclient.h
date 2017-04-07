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
#include "../ccontext.h"
#include "exception.hpp"

class CClient {
	private:
		std::string m_working_dir;
		boost::shared_ptr<boost::asio::io_service> m_io_service;
	public:
		explicit CClient(const std::string& working_dir, const std::string& data_subdir);
		~CClient() {}
		CContext* create_context();
		static void connect(CContext* context, const std::string& server, int port,
				    const std::string& login, const std::string& password);
		static void invoke(CContext* context, utility::ICommand* cmd, int datatype);
		static std::string get_hash(CCmdGetMD5* cmd);
		static bool check_integrity(CContext* context, const std::string& srv_filename, 
					    const std::string& cli_filename, int datatype);
};

#endif //CLIENT_H
