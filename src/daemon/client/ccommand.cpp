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

		boost::shared_ptr<boost::array<int, 3>> msg(new boost::array<int, 3>);
		msg[0] = static_cast<int>(ECommand::GET_FILE);
		msg[1] = static_cast<int>(m_filename.size());
		msg[2] = static_cast<int>(datatype);

		context->socket_write<boost::array<int, 3>>(msg);
		context->socket_write<std::string>(m_filename);

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

	boost::shared_ptr<boost::array<int, 3>> msg(new boost::array<int, 3>);
	msg[0] = static_cast<int>(ECommand::GET_MD5);
	msg[1] = static_cast<int>(m_filename.size());
	msg[2] = static_cast<int>(datatype);

	context->socket_write<boost::array<int, 3>>(*msg);
	context->socket_write<std::string>(m_filename);

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

md5sum_ptr CCmdGetMD5::hash() const {
	return m_hash;

}

CCmdUploadFile::CCmdUploadFile(const std::list<std::string>& args) {
	if (args.size() != EXPECTED_ARGS_NUM) {
		throw ExInvalidArgs("Invalid number of arguments", "CCmdUploadFile::CCmdUploadFile()");
	}
	m_filename = args.front();
}

ECommand CCmdUploadFile::type() const {
	return ECommand::UPLOAD_FILE;
}

EError CCmdUploadFile::invoke(CContext* context, EDataType datatype) {
	boost::shared_ptr<boost::array<int, 3>> msg(new boost::array<int, 3>);
	msg[0] = static_cast<int>(ECommand::UPLOAD_FILE);
	msg[1] = static_cast<int>(m_filename.size());
	msg[2] = static_cast<int>(datatype);

	context->socket_write<boost::array<int, 3>>(*msg);
	context->socket_write<std::string>(m_filename);

	std::vector<char> data_buf;
	EError ret = datatype_instance->get_data(data_buf, log);
	delete datatype_instance;

	return context->socket_write<std::vector<char>>(data_buf);
}
