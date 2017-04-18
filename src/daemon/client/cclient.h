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
		boost::shared_ptr<CContext> m_context;
	public:
		explicit CClient(const std::string& working_dir, const std::string& data_subdir);
		~CClient() {}
		void connect(const std::string& server, int port,
			     const std::string& login, const std::string& password);
		void invoke(utility::ICommand* cmd, int datatype);
		static std::string get_hash(CCmdGetMD5* cmd);
		bool check_integrity(const std::string& srv_filename,
				     const std::string& cli_filename, int datatype);
};

#endif //CLIENT_H
