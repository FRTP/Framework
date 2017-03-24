#include "datatype.h"

using namespace datatypes;

std::map<EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		m_filename = args.front();
	}
}

EError CDataTypeShares::get_data(std::vector<char>& output) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

EError CDataTypeShares::append_data(std::vector<char>& output) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);

	int i = output.size();
	output.resize(output.size() + size);
	while (!file.eof()) {
		file >> output[i++];
	}
	file.close();

	return EError::OK;
}

bool CDataTypeShares::success() const {
	return m_success;
}

EError CDataTypeShares::write_data(std::vector<char>::const_iterator begin,
				   std::vector<char>::const_iterator end) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ofstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	while (begin != end) {
		file << *begin++;
	}
	file.close();

	return EError::OK;
}

std::string CDataTypeShares::full_path() const {
	return (CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
}

std::string CDataTypeShares::path() const {
	return (CSettings::working_dir() + "data/" + m_data_dir);
}

CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		m_filename = args.front();
	}
}

EError CDataTypeTwitter::get_data(std::vector<char>& output) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

EError CDataTypeTwitter::append_data(std::vector<char>& output) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);

	int i = output.size();
	output.resize(output.size() + size);
	while (!file.eof()) {
		file >> output[i++];
	}
	file.close();

	return EError::OK;
}

bool CDataTypeTwitter::success() const {
	return m_success;
}

EError CDataTypeTwitter::write_data(std::vector<char>::const_iterator begin,
				    std::vector<char>::const_iterator end) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ofstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	while (begin != end) {
		file << *begin;
	}
	file.close();

	return EError::OK;
}

std::string CDataTypeTwitter::full_path() const {
	return (CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
}

std::string CDataTypeTwitter::path() const {
	return (CSettings::working_dir() + "data/" + m_data_dir);
}
