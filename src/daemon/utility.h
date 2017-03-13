#ifndef UTILITY_H
#define UTILITY_H

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <openssl/md5.h>
#include <string>

namespace utility {
	typedef boost::array<unsigned char, MD5_DIGEST_LENGTH> md5sum;
	typedef boost::shared_ptr<md5sum> md5sum_ptr;

	enum class ECommand {
		GET_FILE,
		GET_MD5,
		UPLOAD_FILE
	};

	enum class EError {
		OK,
		READ_ERROR,
		OPEN_ERROR,
		WRITE_ERROR,
		SOCKET_ERROR,
		UNKNOWN_ERROR,
		MAX_VAL = UNKNOWN_ERROR
	};

	enum class EDataType {
		SHARES,
		TWITTER,
		MAX_VAL = TWITTER
	};

	std::string get_text_error(EError error);
	std::string get_data_type_dir(EDataType type); 
	md5sum_ptr calculate_md5(const std::string& full_path);

	class CSettings {
		private:
			static std::string m_working_dir;
		public:
			static void set_working_dir(const std::string& working_dir);
			static std::string working_dir();
	};
};

#endif //UTILITY_H
