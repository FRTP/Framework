#include "datatype.h"

using namespace datatypes;

std::map<EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

CDataTypeShares::CDataTypeShares(const std::list<std::string>& args) {
	m_success = (args.size() == EXPECTED_ARGS_NUM);
	if (m_success) {
		_arg_to_esource(args.front());
		m_filename = *(std::next(args.begin(), 1));
	}
}

void CDataTypeShares::_arg_to_esource(const std::string& input) {
	if (input == "YAHOO") {
		m_source = ESource::YAHOO;
	}
	else if (input == "BSC") {
		m_source = ESource::BCS;
	}
	else if (input == "FAST") {
		m_source = ESource::FAST;
	}
	else {
		m_source = ESource::NO_SOURCE;
	}
}

EError CDataTypeShares::_from_bcs(data_t& output, bool append) const {
	std::string full_path = get_full_path(EDataType::SHARES, m_filename);
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

EError CDataTypeShares::_from_yahoo(data_t& output, bool append) const {
	//TODO: add caching
	std::string request = "http://ichart.finance.yahoo.com/table.csv?s=";
	auto delimiter_pos = m_filename.find("/");
	std::string shares = m_filename.substr(0, delimiter_pos);
	std::string date_interval = m_filename.substr(++delimiter_pos, m_filename.end());
	request += shares + "&" + date_interval + "&g=d&ignore=.csv";

	std::ostringstream oss;
	oss << curlpp::options::Url(std::string(request));
	str_to_data_t(oss.str(), output, append);
}

EError CDataTypeShares::_from_fast(__attribute__ ((unused)) data_t& output, __attribute__ ((unused)) bool append) const {
	//TODO
	return EError::INTERNAL_ERROR;
}

EError CDataTypeShares::get_data(data_t& output, bool append = false) const {
	switch (m_source) {
		case ESource::BCS:
			return _from_bcs(output, append);
		case ESource::YAHOO:
			return _from_yahoo(output, append);
		case ESource::FAST:
			return _from_fast(output, append);
		default:
			return EError::INTERNAL_ERROR;
	}
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

EError CDataTypeTwitter::get_data(data_t& output, bool append = false) const {
	std::string full_path = get_full_path(EDataType::TWITTER, m_filename);
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
