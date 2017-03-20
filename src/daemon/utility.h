#ifndef UTILITY_H
#define UTILITY_H

#include <boost/array.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <openssl/md5.h>
#include <sstream>
#include <string>
#include <vector>

namespace utility {
	typedef boost::array<unsigned char, MD5_DIGEST_LENGTH> md5sum;
	typedef boost::shared_ptr<md5sum> md5sum_ptr;

	enum class ECommand {
		FEEDBACK,
		GET_FILE,
		GET_MD5,
		UPLOAD_FILE,
		MAX_VAL = UPLOAD_FILE
	};

	enum class EError {
		OK,
		READ_ERROR,
		OPEN_ERROR,
		WRITE_ERROR,
		SOCKET_ERROR,
		UNKNOWN_COMMAND,
		UNKNOWN_DATATYPE,
		CORRUPTED_MESSAGE,
		UNKNOWN_ERROR,
		MAX_VAL = UNKNOWN_ERROR
	};

	enum class EDataType {
		ERROR_CODE,
		SHARES,
		TWITTER,
		MAX_VAL = TWITTER
	};

	std::string get_text_error(EError error);
	std::string get_data_type_dir(EDataType type); 
	md5sum_ptr calculate_md5(const std::string& full_path);
	std::string md5sum_to_str(md5sum_ptr md5);

	class CSettings {
		private:
			static std::string m_working_dir;
		public:
			static void set_working_dir(const std::string& working_dir);
			static std::string working_dir();
	};

	class CMessage {
		private:
			ECommand m_cmd;
			EDataType m_datatype;
			std::vector<char> m_data;
			md5sum m_hash;

			void _calculate_hash();
		public:
			CMessage();
			CMessage(ECommand cmd, EDataType datatype, const std::vector<char>& data);
			void to_streambuf(boost::asio::streambuf& buffer) const;
			EError from_streambuf(boost::asio::streambuf& buffer);
			ECommand command() const;
			EDataType datatype() const;
			std::vector<char>& data();
	};
};

#endif //UTILITY_H
