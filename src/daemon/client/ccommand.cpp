#include "ccommand.h"

std::map<std::string, CCommandCreator*> CCommandFactory::m_factory;

CCmdGetFile::CCmdGetFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetFile::CCmdGetFile()");
	}
	m_filename = *(args.begin());
	m_newfilename = *(std::next(args.begin(), 1));
	m_force_update = (*(std::next(args.begin(), 2)) == "True") ? true : false;
}

ECommand CCmdGetFile::type() const {
	return ECommand::GET_FILE;
}

EError CCmdGetFile::invoke(CContext* context, EDataType datatype) {
	std::string full_path = CSetting::working_dir() + get_data_type_dir(datatype) + 
				"/" + m_newfilename;
	if(!fs::exists(full_path) || m_force_update) {
		fs::create_directories(CSettings::working_dir() + get_data_type_dir(datatype) + "/");
		int msg[3] = { static_cast<int>(ECommand::GET_FILE), 
			       static_cast<int>(m_filename.size()),
			       static_cast<int>(datatype) };
		context->socket_write<int*>(msg, 3 * sizeof(int));
		context->socket_write<std::string>(m_filename, msg[1]);

		streambuf receive_buffer;
		context->socket_read(receive_buffer);
		if (receive_buffer.size() == 1) {
			const int* error_buff = buffer_cast<const int*>(receive_buffer.data());
			if (*error_buff < 0 || *error_buf >= static_cast<int>(EError::MAX_VAL)) {
				return EError::UNKNOWN_ERROR;
			}
			return static_cast<EError>(*error_buf);
		}
		const char* data = buffer_cast<const char*>(receive_buffer.data());
		std::string full_path = CSettings::working_dir() + get_data_type_dir(datatype) + 
					"/" + m_newfilename;
		std::ofstream out(full_path, std::ios::binary);
		out << data;

		out.close();
	}
	return EError::OK;
}

CCmdGetMD5::CCmdGetMD5(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdGetMD5::CCmdGetMD5()");
	}
	m_filename = args.front();
}

ECommand CCmdGetMD5::type() const {
	return ECommand::GET_MD5;
}

EError CCmdGetMD5::invoke(CContext* context, EDataType datatype) {
	std::string full_path = CSetting::working_dir() + get_data_type_dir(datatype) + 
				"/" + m_filename;
	if (!fs::exists(full_path)) {
		throw ExNoFile("Invalid path " + full_path, "CCmdGetMD5::invoke()");
	}
	int msg[3] = { static_cast<int>(ECommand::GET_MD5), 
		       static_cast<int>(m_filename.size()),
		       static_cast<int>(datatype) };
	context->socket_write<int*>(msg, 3 * sizeof(int));
	context->socket_write<std::string>(m_filename, msg[1]);

	streambuf receive_buffer;
	context->socket_read(receive_buffer);
	if (receive_buffer.size() == 1) {
		const int* error_buff = buffer_cast<const int*>(receive_buffer.data());
		if (*error_buff < 0 || *error_buf >= static_cast<int>(EError::MAX_VAL)) {
			return EError::UNKNOWN_ERROR;
		}
		return static_cast<EError>(*error_buf);
	}

	m_hash = buffer_cast<const unsigned char*>(receive_buffer.data());
	m_hash_size = receive_buffer.size();

	return EError::OK;
}

const unsigned char* CCmdGetMD5::hash(int& size) {
	size = m_hash_size;
	return m_hash;

}
