#include "datatype.h"

using namespace datatypes;

std::map<EDataType, CDataTypeCreator*> CDataTypeFactory::m_types;

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success} {
		m_filename = args.front();
	}
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

bool CDataTypeShares::success() const {
	return m_success;
}

CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success} {
		m_filename = args.front();
	}
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

bool CDataTypeTwitter::success() const {
	return m_success;
}
