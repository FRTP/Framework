#include "utility.h"

namespace utility {
	std::string CSettings::m_working_dir;

	std::string get_text_error(EError error) {
		std::string res = "";
		switch (error) {
			case EError::OK:
				res = "No errors";
				break;
			case EError::READ_ERROR:
				res = "Unable to read file";
				break;
			case EError::OPEN_ERROR:
				res = "Unable to open file";
				break;
			case EError::WRITE_ERROR:
				res = "Unable to write file";
				break;
			case EError::SOCKET_ERROR:
				res = "Socket error";
				break;
			case EError::UNKNOWN_ERROR:
				res = "Unknown error";
				break;
		}
		return res;
	}

	std::string get_data_type_dir(EDataType type) {
		switch (type) {
			case EDataType::SHARES:
				return std::string("shares");
			case EDataType::TWITTER:
				return std::string("twitter");
			default:
				return std::string("");
		}
	}

	md5sum_ptr calculate_md5(const std::string& full_path) {
		md5sum_ptr md5(new md5sum);
		std::ifstream in(full_path);
		std::string content;
		in >> content;
		MD5((unsigned char*)content.data(), content.size(), md5->data());
		return md5;
	}

	void CSettings::set_working_dir(const std::string& working_dir) {
		m_working_dir = working_dir;
	}

	std::string CSettings::working_dir() {
		return m_working_dir;
	}
};
