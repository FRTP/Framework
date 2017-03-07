#include "ccommand.h"

std::map<std::string, IAbstractCommandCreator*> CCommandFactory::m_factory;

CCmdGetFile::CCmdGetFile(const std::list<std::string>& args) {
	if (args.size() != 2) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetFile::CCmdGetFile()");
	}
	m_filename = *(args.begin());
	m_newfilename = *(std::next(args.begin(), 1));
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

void CCmdGetFile::invoke(CContext* context, int& error) {
	int msg[2] = { static_cast<int>(ECommand::GET_FILE), static_cast<int>(m_filename.size()) };
	context->socket_write<int*>(msg, 2);
	context->socket_write<std::string>(m_filename, msg[1]);

	streambuf receive_buffer;
	context->socket_read(receive_buffer);
	if (receive_buffer.size() == 1) {
		const int* error_buff = buffer_cast<const int*>(receive_buffer.data());
		error = *error_buff;
		return;
	}
	const char* data = buffer_cast<const char*>(receive_buffer.data());
	std::ofstream out(m_newfilename, std::ios::binary);
	out << data;
	out.close();
	error = 0;
}

CCmdGetMD5::CCmdGetMD5(const std::list<std::string>& args) {
	if (args.size() != 1) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetMD5::CCmdGetMD5()");
	}
	m_filename = args.front();
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

void CCmdGetMD5::invoke(CContext* context, int& error) {
	int msg[2] = { static_cast<int>(ECommand::GET_MD5), static_cast<int>(m_filename.size()) };
	context->socket_write<int*>(msg, 2);
	context->socket_write<std::string>(m_filename, msg[1]);

	streambuf receive_buffer;
	context->socket_read(receive_buffer);
	if (receive_buffer.size() == 1) {
		const int* error_buff = buffer_cast<const int*>(receive_buffer.data());
		error = *error_buff;
		return;
	}

	m_hash = buffer_cast<const unsigned char*>(receive_buffer.data());
	m_hash_size = receive_buffer.size();
	error = 0;
}

const unsigned char* CCmdGetMD5::hash(int& size) {
	size = m_hash_size;
	return m_hash;

}
