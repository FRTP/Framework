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

		auto begin_write_prt = append ? output.data() : output.data() + prev_size;
		if (!file.read(reinterpret_cast<char*>(begin_write_prt), file_size)) {
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

		return utility::EError::OK;
	}

	CDataTypeShares::CDataTypeAssets(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			auto delimiter_pos = args.front().find("\n");
			m_source = args.front().substr(0, delimiter_pos);
			m_filename = args.front().substr(++delimiter_pos, args.front().end());
		}
	}

	utility::EError CDataTypeAssets::get_data(utility::data_t& output, bool append = false) const {
		std::string full_path = utility::get_full_path(utility::EDataType::ASSETS,
							       m_source + "/" + m_filename);
		return read_binary(output, full_path, append);
	}

	bool CDataTypeAssets::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeAssets::write_data(utility::data_t::const_iterator begin,
						    utility::data_t::const_iterator end) const {
		std::string full_path = utility::get_full_path(utility::EDataType::ASSETS, m_filename);
		return write_binary(full_path, begin, end);
	}

	std::string CDataTypeAssets::get_full_path() const {
		return utility::get_full_path(utility::EDataType::ASSETS, m_source + "/" + m_filename);
	}

	std::string CDataTypeAssets::get_path() const {
		return (utility::CSettings::data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS));
	}

	CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			m_filename = args.front();
		}
	}

	utility::EError CDataTypeTwitter::get_data(data_t& output, bool append = false) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		return read_binary(output, full_path, append);
	}

	bool CDataTypeTwitter::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeTwitter::write_data(utility::data_t::const_iterator begin,
					    utility::data_t::const_iterator end) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		return write_binary(full_path, begin, end);
	}

	std::string CDataTypeTwitter::get_full_path() const {
		return utility::get_full_path(utility::EDataType::TWITTER, m_filename);
	}

	std::string CDataTypeTwitter::get_path() const {
		return (utility::CSettings::data_dir() +
			utility::get_data_type_dir(utility::EDataType::TWITTER));
	}
}
