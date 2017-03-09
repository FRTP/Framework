#include "datatype.h"

using namespace datatypes;

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	if(args.size() != EXPECTED_ARGS_NUM) {
	}
	m_filename = args.front();
}

EError CDataTypeShares::get_data(std::vector<char>& output, boost::shared_ptr<CLog> log) {
	std::string full_path("/var/frtp/data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		log->write("[EE]: Unable to open file " + full_path + ": " + strerror(errno));
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		log->write("[EE]: Unable to read file " + full_path);
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
	if(args.size() != EXPECTED_ARGS_NUM) {
	}
	m_filename = args.front();
}

EError CDataTypeTwitter::get_data(std::vector<char>& output, boost::shared_ptr<CLog> log) {
	std::string full_path("/var/frtp/data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		log->write("[EE]: Unable to open file " + full_path + ": " + strerror(errno));
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		log->write("[EE]: Unable to read file " + full_path);
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}
