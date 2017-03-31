#include "datatype.h"

using namespace datatypes;

std::map<EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

EError read_binary(data_t& output, const std::string& full_path, bool append) {
	std::ifstream file(full_path, std::ios::binary);
	if (!file) {
		return EError::OPEN_ERROR;
	}

	file.seekg(0, file.end);
	int file_size = file.tellg();
	file.seekg(0, file.beg);
	int prev_size = output.size();
	output.resize(append ? prev_size + file_size : prev_size);

	auto begin_write_prt = append ? output.data() : output.data() + prev_size;
	if (!file.read(reinterpret_cast<char*>(begin_write_prt), file_size)) {
		file.close();
		return EError::READ_ERROR;
	}

	file.close();
	return EError::OK;
}

EError write_binary(const std::string& full_path, data_t::const_iterator begin,
		    data_t::const_iterator end) {
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

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		auto delimiter_pos = args.front().find("\n");
		m_source = args.front().substr(0, delimiter_pos);
		m_filename = args.front().substr(++delimiter_pos, args.front().end());
	}
}

EError CDataTypeShares::get_data(data_t& output, bool append = false) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_source + "/" + m_filename);
	return read_binary(output, full_path, append);
}

bool CDataTypeShares::success() const {
	return m_success;
}

EError CDataTypeShares::write_data(data_t::const_iterator begin,
				   data_t::const_iterator end) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_filename);
	return write_binary(full_path, begin, end);
}

std::string CDataTypeShares::full_path() const {
	return get_full_path(EDataType::SHARES, m_source + "/" + m_filename);
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

EError CDataTypeTwitter::get_data(data_t& output, bool append = false) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
	return read_binary(output, full_path, append);
}

bool CDataTypeTwitter::success() const {
	return m_success;
}

EError CDataTypeTwitter::write_data(data_t::const_iterator begin,
				    data_t::const_iterator end) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
	return write_binary(full_path, begin, end);
}

std::string CDataTypeTwitter::full_path() const {
	return get_full_path(EDataType::TWITTER, m_filename);
}

std::string CDataTypeTwitter::path() const {
	return (CSettings::data_dir() + get_data_type_dir(EDataType::TWITTER));
}
