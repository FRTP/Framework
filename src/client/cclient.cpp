#include "cclient.h"

CClient::CClient(boost::asio::io_service& io_service, char* server, int port) 
	: m_socket(io_service), m_server(server), m_port(port) {
	m_socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(m_server), m_port));
}

const char* CClient::get_file_list() {
	if(m_socket.is_open()) {
		boost::system::error_code error;
		int msg[1] = { ECommand::GET_FILE_LIST };
		asio::write(m_socket, asio::buffer(msg), error);
		if(error) 
			return NULL;
		asio::streambuf receive_buffer;
		asio::read(m_socket, receive_buffer, asio::transfer_all(), error);
		if(error && error != asio::error::eof)
			return NULL;
		else {
			const char* data = asio::buffer_cast<const char*>(receive_buffer.data());
			return data;
		}
	}
	else
		return NULL;
}

int CClient::get_file(char* filename, const char* newfile) {
	if(m_socket.is_open()) {
		boost::system::error_code error;
		std::string file(filename);
		int msg[2] = { ECommand::GET_FILE, file.size() };
		asio::write(m_socket, asio::buffer(msg), error);
		if(error) 
			return -1;
		asio::write(m_socket, asio::buffer(file), error);
		if(error)
			return -1;

		asio::streambuf receive_buffer;
		asio::read(m_socket, receive_buffer, asio::transfer_all(), error);
		if(error && error != asio::error::eof)
			return -2;
		else {
			char* data = asio::buffer_cast<char*>(receive_buffer.data());
			std::ofstream out(newfile, std::ios::binary);
			out << data;
			out.close();
			return 0;
		}
	}
	else
		return -3;
}
