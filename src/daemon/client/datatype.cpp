#include "datatype.h"

namespace datatypes {
	std::map<utility::EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

	utility::EError read_binary(utility::data_t& output, const std::string& full_path, bool append) {
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int file_size = file.tellg();
		file.seekg(0, file.beg);
		int prev_size = output.size();
		output.resize(append ? prev_size + file_size : prev_size);

		auto begin_write_ptr = append ? output.data() + prev_size : output.data();
		if (!file.read(reinterpret_cast<char*>(begin_write_ptr), file_size)) {
			file.close();
			return utility::EError::READ_ERROR;
		}

		file.close();
		return utility::EError::OK;
	}

	utility::EError write_binary(const std::string& full_path,
				     utility::data_t::const_iterator begin,
				     utility::data_t::const_iterator end) {
		std::ofstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		while (begin != end) {
			file << *begin++;
		}
		file.close();

		if (file.fail()) {
			return utility::EError::WRITE_ERROR;
		}

		return utility::EError::OK;
	}

	CDataTypeAssets::CDataTypeAssets(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			auto delimiter_pos = args.front().find("\t");
			m_source = args.front().substr(0, delimiter_pos);
			m_filename = args.front().substr(++delimiter_pos);
		}
	}

	utility::EError CDataTypeAssets::get_data(utility::data_t& output, bool append = false) const {
		return read_binary(output, get_full_path(), append);
	}

	bool CDataTypeAssets::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeAssets::write_data(utility::data_t::const_iterator begin,
						    utility::data_t::const_iterator end) const {
		return write_binary(get_full_path(), begin, end);
	}

	std::string CDataTypeAssets::get_full_path() const {
		return utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS) +
		       m_source + "/" + m_filename;
	}

	std::string CDataTypeAssets::get_path() const {
		auto delimiter_pos = m_filename.find("/");
		std::string subdir = m_filename.substr(0, ++delimiter_pos);
		return (utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS) +
			m_source + "/" + subdir);
	}

	CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			m_filename = args.front();
		}
	}

	utility::EError CDataTypeTwitter::get_data(utility::data_t& output, bool append = false) const {
		return read_binary(output, get_full_path(), append);
	}

	bool CDataTypeTwitter::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeTwitter::write_data(utility::data_t::const_iterator begin,
						     utility::data_t::const_iterator end) const {
		return write_binary(get_full_path(), begin, end);
	}

	std::string CDataTypeTwitter::get_full_path() const {
		return utility::get_full_path(utility::EDataType::TWITTER, m_filename);
	}

	std::string CDataTypeTwitter::get_path() const {
		return (utility::CSettings::get_data_dir() +
			utility::get_data_type_dir(utility::EDataType::TWITTER));
	}
}
