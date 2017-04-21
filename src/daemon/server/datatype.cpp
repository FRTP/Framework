#include "datatype.h"

namespace datatypes {
	std::map<utility::EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

	CDataTypeAssets::CDataTypeAssets(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			auto delimiter_pos = args.front().find("\t");
			m_source = args.front().substr(0, delimiter_pos);
			m_filename = args.front().substr(++delimiter_pos);
		}
	}

	CDataTypeAssets::ESource CDataTypeAssets::_arg_to_esource(const std::string& input) const {
		if (input == "YAHOO") {
			return ESource::YAHOO;
		}
		else if (input == "BCS") {
			return ESource::BCS;
		}
		else if (input == "FAST") {
			return ESource::FAST;
		}
		else {
			return ESource::NO_SOURCE;
		}
	}

	utility::EError CDataTypeAssets::_from_bcs(utility::data_t& output, bool append) const {
		std::ifstream file(get_full_path(), std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int file_size = file.tellg();
		file.seekg(0, file.beg);
		int prev_size = output.size();
		output.resize(append ? prev_size + file_size : file_size);

		auto begin_write_ptr = append ? output.data() + prev_size : output.data();
		file.read(reinterpret_cast<char*>(begin_write_ptr), file_size);
		file.close();
		if (file.fail()) {
			return utility::EError::READ_ERROR;
		}

		return utility::EError::OK;
	}

	utility::EError CDataTypeAssets::_from_yahoo(utility::data_t& output, bool append) const {
		std::string request = "https://ichart.finance.yahoo.com/table.csv?s=";
		auto delimiter_pos = m_filename.find("/");
		std::string shares = m_filename.substr(0, delimiter_pos);
		std::string date_interval = m_filename.substr(++delimiter_pos);
		request += shares + "&" + date_interval + "&g=d&ignore=.csv";

		std::ostringstream oss;
		oss << curlpp::options::Url(std::string(request));
		utility::str_to_data_t(oss.str(), output, append);

		return utility::EError::OK;
	}

	utility::EError CDataTypeAssets::_from_fast(__attribute__ ((unused)) utility::data_t& output,
						    __attribute__ ((unused)) bool append) const {
		//TODO
		return utility::EError::INTERNAL_ERROR;
	}

	utility::EError CDataTypeAssets::get_data(utility::data_t& output, bool append) const {
		switch (_arg_to_esource(m_source)) {
			case ESource::BCS:
				return _from_bcs(output, append);
			case ESource::YAHOO:
				return _from_yahoo(output, append);
			case ESource::FAST:
				return _from_fast(output, append);
			default:
				return utility::EError::INTERNAL_ERROR;
		}
	}

	bool CDataTypeAssets::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeAssets::write_data(utility::data_t::const_iterator begin,
						    utility::data_t::const_iterator end) const {
		std::ofstream file(get_full_path(), std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		while (begin != end) {
			file << *begin++;
		}
		file.close();

		return utility::EError::OK;
	}

	std::string CDataTypeAssets::get_full_path() const {
		return utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS) +
		       m_source + "/" + m_filename;
	}

	std::string CDataTypeAssets::get_path() const {
		auto delimiter_pos = m_filename.find("/");
		std::string subdir = m_filename.substr(0, delimiter_pos);

		return (utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS) +
			m_source + "/" + subdir);
	}

	CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			m_filename = args.front();
		}
	}

	utility::EError CDataTypeTwitter::get_data(utility::data_t& output, bool append) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int file_size = file.tellg();
		file.seekg(0, file.beg);
		int prev_size = output.size();
		output.resize(append ? prev_size + file_size : prev_size);

		auto begin_write_prt = append ? output.data() : output.data() + prev_size;
		if (!file.read(reinterpret_cast<char*>(begin_write_prt), file_size)) {
			file.close();
			return utility::EError::READ_ERROR;
		}

		file.close();
		return utility::EError::OK;
	}

	bool CDataTypeTwitter::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeTwitter::write_data(utility::data_t::const_iterator begin,
						     utility::data_t::const_iterator end) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		std::ofstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		while (begin != end) {
			file << *begin;
		}
		file.close();

		return utility::EError::OK;
	}

	std::string CDataTypeTwitter::get_full_path() const {
		return utility::get_full_path(utility::EDataType::TWITTER, m_filename);
	}

	std::string CDataTypeTwitter::get_path() const {
		return (utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::TWITTER));
	}
}
