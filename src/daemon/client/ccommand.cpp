#include "ccommand.h"

CCmdGetFile::CCmdGetFile(const std::string& filename, const std::string& newfilename) {
	m_filename = filename;
	m_newfilename = newfilename;
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

void CCmdGetFile::invoke(const std::shared_ptr<CContext>& context, int& error) {
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

CCmdGetMD5::CCmdGetMD5(const std::string& filename) {
	m_filename = filename;
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

void CCmdGetMD5::invoke(const std::shared_ptr<CContext>& context, int& error) {
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
	error = 0;
}

const unsigned char* CCmdGetMD5::hash() {
	return m_hash;
}
