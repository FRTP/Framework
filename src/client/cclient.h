#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>

using namespace boost::asio;

class CClient {
	private:
		ip::tcp::socket m_socket;
		std::string m_server;
		int m_port;
	public:
		enum ECommand {
			GET_FILE,
			GET_FILE_LIST
		};

		CClient(io_service& io_service, char* server, int port);
		const char* get_file_list();
		int get_file(char* filename, const char* newfile);
};

#endif //CLIENT_H
