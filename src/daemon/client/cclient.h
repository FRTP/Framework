#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>

using namespace boost::asio;

class CClient {
	public:
		enum EServerError {
			NO_FILE
		};
		enum ECommand {
			GET_FILE,
		};
		enum  EError {
			WRITE_ERROR,
			READ_ERROR, 
			SOCKET_ERROR
		};

	private:
		ip::tcp::socket m_socket;
		std::string m_server;
		int m_port;
		std::string _get_text_error(EServerError error) const;

	public:
		CClient(io_service& io_service, char* server, int port);
		~CClient();
		int get_file(char* filename, const char* newfile);
};

#endif //CLIENT_H
