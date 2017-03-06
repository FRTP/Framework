#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <memory>

#include "ccommand.h"
#include "ccontext.hpp"

using namespace boost::asio;

class CClient {
	public:
		enum EServerError {
			NO_FILE
		};

	private:
		std::shared_ptr<CContext> m_context;
		std::string _get_text_error(EServerError error) const;
	public:
		CClient(io_service& io_service, char* server, int port);
		~CClient() {}
		void invoke(ICommand* cmd);
};

#endif //CLIENT_H
