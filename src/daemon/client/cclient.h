#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>
#include <memory>

#include "ccommand.h"
#include "ccontext.hpp"
#include "exception.hpp"

using namespace boost::asio;

class CClient {
	public:
		enum EServerError {
			NO_FILE
		};

	private:
		std::string m_server;
		int m_port;
		boost::shared_ptr<io_service> m_io_service;

		static std::string _get_text_error(EServerError error);
	public:
		CClient(const std::string& server, int port);
		~CClient() {}
		CContext* create_context();
		static void connect(CContext* context);
		static int invoke(CContext* context, ICommand* cmd);
		static boost::python::list get_hash(CCmdGetMD5* cmd);
};

#endif //CLIENT_H
