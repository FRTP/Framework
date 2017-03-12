#include "datatype.h"

using namespace datatypes;

std::map<EDataType, IAbstractDataTypeCreator*> CDataTypeFactory::m_types;

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
		BOOST_LOG_TRIVIAL(error) << "unable to open file " + full_path + ": " + strerror(errno);
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		BOOST_LOG_TRIVIAL(error) << "unable to read file " + full_path;
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

bool CDataTypeShares::success() const {
	return m_success;
}

EError CDataTypeShares::write_data(const std::vector<char>& input) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ofstream file(full_path, std::ios::binary);
	if (!file) {
		BOOST_LOG_TRIVIAL(error) << "unable to read file " + full_path;
		return EError::OPEN_ERROR;
	}

	if (!file.write(input.data(), input.size())) {
		BOOST_LOG_TRIVIAL(error) << "unable to write file " + full_path;
		file.close();
		return EError::WRITE_ERROR;
	}

	file.close();
	return EError::OK;
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
		BOOST_LOG_TRIVIAL(error) << "unable to open file " + full_path + ": " + strerror(errno);
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	output.resize(size);

	if (!file.read(output.data(), size)) {
		BOOST_LOG_TRIVIAL(error) << "unable to read file " + full_path;
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

bool CDataTypeTwitter::success() const {
	return m_success;
}

EError CDataTypeTwitter::write_data(const std::vector<char>& input) const {
	std::string full_path(CSettings::working_dir() + "data/" + m_data_dir + "/" + m_filename);
	std::ofstream file(full_path, std::ios::binary);
	if (!file) {
		BOOST_LOG_TRIVIAL(error) << "unable to open file " + full_path + ": " + strerror(errno);
		return EError::OPEN_ERROR;
	}

	if (!file.write(input.data(), input.size())) {
		BOOST_LOG_TRIVIAL(error) << "unable to write file " + full_path;
		file.close();
		return EError::WRITE_ERROR;
	}

	file.close();
	return EError::OK;
}
