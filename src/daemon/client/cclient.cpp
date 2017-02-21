#include "cclient.h"

CClient::CClient(io_service& io_service, char* server, int port) 
	: m_socket(io_service), m_server(server), m_port(port) {
		boost::system::error_code error;
		m_socket.connect(ip::tcp::endpoint(ip::address::from_string(m_server), m_port), error);
		if(error)
			std::cerr << "Unable to connect" << std::endl;
		else
			std::cout << "Connected" << std::endl;
}

CClient::~CClient() {
	m_socket.close();
}

const char* CClient::get_file_list() {
	if(m_socket.is_open()) {
		boost::system::error_code error;
		int msg[2] = { ECommand::GET_FILE_LIST, 0 };
		write(m_socket, buffer(msg), error);
		if(error) 
			return NULL;
		streambuf receive_buffer;
		read(m_socket, receive_buffer, transfer_all(), error);
		if(error && error != error::eof)
			return NULL;
		else {
			const char* data = buffer_cast<const char*>(receive_buffer.data());
			return data;
		}
	}
	else
		return NULL;
}

int CClient::get_file(char* filename, const char* newfilename) {
	if(m_socket.is_open()) {
		boost::system::error_code error;
		std::string file(filename);
		int msg[2] = { static_cast<int>(ECommand::GET_FILE), static_cast<int>(file.size()) };
		write(m_socket, buffer(msg), error);
		if(error) 
			return -1;
		write(m_socket, buffer(file), error);
		if(error)
			return -1;

		streambuf receive_buffer;
		read(m_socket, receive_buffer, transfer_all(), error);
		if(error && error != error::eof)
			return -2;
		else {
			const char* data = buffer_cast<const char*>(receive_buffer.data());
			std::ofstream out(newfilename, std::ios::binary);
			out << data;
			out.close();
			return 0;
		}
	}
	else {
		std::cerr << "Socket is closed" << std::endl;
		return -3;
	}
}
