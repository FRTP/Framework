#include "datatype.h"

using namespace datatypes;

std::map<EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		m_filename = args.front();
	}
}

EError CDataTypeShares::get_data(data_t& output) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(reinterpret_cast<char*>(output.data()), size)) {
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

EError CDataTypeShares::append_data(data_t& output) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);

	int pre_size = output.size();
	output.resize(pre_size + size);
	if (!file.read(reinterpret_cast<char*>(output.data() + pre_size), size)) {
		file.close();
		return EError::READ_ERROR;
	}
	file.close();

	return EError::OK;
}

bool CDataTypeShares::success() const {
	return m_success;
}

EError CDataTypeShares::write_data(data_t::const_iterator begin,
				   data_t::const_iterator end) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_filename);
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
	return get_full_path(EDataType::SHARES, m_filename);
}

std::string CDataTypeShares::path() const {
	return (CSettings::data_dir() + get_data_type_dir(EDataType::SHARES));
}

CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		m_filename = args.front();
	}
}

EError CDataTypeTwitter::get_data(data_t& output) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(reinterpret_cast<char*>(output.data()), size)) {
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

EError CDataTypeTwitter::append_data(data_t& output) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);

	int pre_size = output.size();
	output.resize(pre_size + size);
	if (!file.read(reinterpret_cast<char*>(output.data() + pre_size), size)) {
		file.close();
		return EError::READ_ERROR;
	}
	file.close();

	return EError::OK;
}

bool CDataTypeTwitter::success() const {
	return m_success;
}

EError CDataTypeTwitter::write_data(data_t::const_iterator begin,
				    data_t::const_iterator end) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
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
	return get_full_path(EDataType::TWITTER, m_filename);
}

std::string CDataTypeTwitter::path() const {
	return (CSettings::data_dir() + get_data_type_dir(EDataType::TWITTER));
}
