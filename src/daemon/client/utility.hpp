#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>

namespace utility {
	enum ECommand {
		GET_FILE,
		GET_MD5
	};

	enum EError {
		OK,
		READ_ERROR,
		OPEN_ERROR,
		UNKNOWN_ERROR,
		MAX_VAL = UNKNOWN_ERROR
	};

	enum EDataType {
		SHARES,
		TWITTER,
		MAX_VAL = TWITTER
	};

	std::string get_text_error(EError error) {
		std::string res = "";
		switch (error) {
			case EError::READ_ERROR:
				res = "Unable to read file";
				break;
			case EError::OPEN_ERROR:
				res = "Unable to open file";
				break;
			case EError::OK:
				res = "No errors";
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
		}
	}

	unsigned char* calculate_md5(const std::string& full_path, int size) {
		unsigned char* md5sum = new unsigned char(MD5_DIGEST_LENGTH);
		//TODO: read file here
		MD5((unsigned char*)data_buf.data(), size, md5sum);
		return md5sum;
	}

	class CSettings {
		private:
			static std::string m_working_dir;
		public:
			static void set_working_dir(const std::string& working_dir) {
				m_working_dir = working_dir;
			}
			static std::string working_dir() {
				return m_working_dir;
			}
	};
};

#endif //UTILITY_HPP
