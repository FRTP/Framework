#include "datatype.h"

namespace datatypes {
	std::map<utility::EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> CDataTypeFactory::m_types;

	CDataTypeAssets::CDataTypeAssets(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			m_filename = args.front();
		}
	}

	utility::EError CDataTypeAssets::get_data(utility::data_t& output) const {
		std::string full_path = utility::get_full_path(utility::EDataType::ASSETS, m_filename);
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);
		output.resize(size);

		if (!file.read(reinterpret_cast<char*>(output.data()), size)) {
			file.close();
			return utility::EError::READ_ERROR;
		}

		file.close();
		return utility::EError::OK;
	}

	utility::EError CDataTypeAssets::append_data(utility::data_t& output) const {
		std::string full_path = utility::get_full_path(utility::EDataType::ASSETS, m_filename);
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);

		int pre_size = output.size();
		output.resize(pre_size + size);
		if (!file.read(reinterpret_cast<char*>(output.data() + pre_size), size)) {
			file.close();
			return utility::EError::READ_ERROR;
		}
		file.close();

		return utility::EError::OK;
	}

	bool CDataTypeAssets::is_success() const {
		return m_success;
	}

	utility::EError CDataTypeAssets::write_data(utility::data_t::const_iterator begin,
						    utility::data_t::const_iterator end) const {
		std::string full_path = utility::get_full_path(utility::EDataType::ASSETS, m_filename);
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

	std::string CDataTypeAssets::get_full_path() const {
		return utility::get_full_path(utility::EDataType::ASSETS, m_filename);
	}

	std::string CDataTypeAssets::get_path() const {
		return (utility::CSettings::get_data_dir() + utility::get_data_type_dir(utility::EDataType::ASSETS));
	}

	CDataTypeTwitter::CDataTypeTwitter(const std::list<std::string>& args) {
		m_success = (args.size() == EXPECTED_ARGS_NUM);
		if (m_success) {
			m_filename = args.front();
		}
	}

	utility::EError CDataTypeTwitter::get_data(utility::data_t& output) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);
		output.resize(size);

		if (!file.read(reinterpret_cast<char*>(output.data()), size)) {
			file.close();
			return utility::EError::READ_ERROR;
		}

		file.close();
		return utility::EError::OK;
	}

	utility::EError CDataTypeTwitter::append_data(utility::data_t& output) const {
		std::string full_path = utility::get_full_path(utility::EDataType::TWITTER, m_filename);
		std::ifstream file(full_path, std::ios::binary);
		if (!file) {
			return utility::EError::OPEN_ERROR;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);

		int pre_size = output.size();
		output.resize(pre_size + size);
		if (!file.read(reinterpret_cast<char*>(output.data() + pre_size), size)) {
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
