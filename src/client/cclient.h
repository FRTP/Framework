#ifndef CCLIENT_H
#define CCLIENT_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>

class CClient {
	private:
		tcp::socket m_socket;
		std::string m_server;
		int m_port;
	public:
		enum ECommand {
			GET_FILE,
			GET_FILE_LIST
		};

		CClient(boost::asio::io_service& io_service);
		const char* get_file_list();
		char* get_file(char* filename, const char* newfile);
};

#endif //CLIENT_H
