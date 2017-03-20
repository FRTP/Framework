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
			case EError::UNKNOWN_COMMAND:
				res = "Unknown command";
				break;
			case EError::UNKNOWN_DATATYPE:
				res = "Unknown data type";
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

		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		std::string content(size, ' ');
		in.seekg(0);
		in.read(&content[0], size);

		MD5((unsigned char*)content.data(), content.size(), md5->data());
		return md5;
	}

	std::string md5sum_to_str(md5sum_ptr md5) {
		std::ostringstream oss;
		for (auto i : *md5) {
			oss << boost::format("%02x") % static_cast<unsigned int>(i);
		}
		oss << std::endl;
		return oss.str();
	}

	void CSettings::set_working_dir(const std::string& working_dir) {
		m_working_dir = working_dir;
	}

	std::string CSettings::working_dir() {
		return m_working_dir;
	}

	CMessage::CMessage(ECommand cmd, EDataType datatype, unsigned int datasize, const std::vector<char>& data,
			   const md5sum& hash) : m_cmd(cmd), m_datatype(datatype), m_datasize(datasize),
						 m_data(data), m_hash(hash) {}

	void CMessage::to_streambuf(boost::asio::streambuf& buffer) const {
		std::ostream out(&buffer);
		out << static_cast<int>(m_cmd) << " " << static_cast<int>(m_datatype) << " " << m_datasize;
		for (auto i : m_data) {
			out << i << " ";
		}
		for (auto i : m_hash) {
			out << i << " ";
		}
	}

	EError CMessage::from_streambuf(boost::asio::streambuf& buffer) {
		std::istream in(&buffer);
		int i_cmd = -1;
		int i_datatype = -1;
		in >> i_cmd >> i_datatype;

		if (i_cmd < 0 && i_cmd > static_cast<int>(ECommand::MAX_VAL)) {
			return EError::UNKNOWN_COMMAND;
		}
		if (i_datatype < 0 && i_datatype > static_cast<int>(EDataType::MAX_VAL)) {
			return EError::UNKNOWN_DATATYPE;
		}

		m_cmd = static_cast<ECommand>(i_cmd);
		m_datatype = static_cast<EDataType>(i_datatype);
		in >> m_datasize;
		m_data.resize(m_datasize);

		char tmp_char;
		for (unsigned int i = 0; i < m_datasize; ++i) {
			in >> tmp_char;
			m_data.push_back(tmp_char);
		}
		char tmp_unsigned_char;
		for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
			in >> tmp_unsigned_char;
			m_hash[i] = tmp_unsigned_char;
		}
		return EError::OK;
	}

	bool CMessage::check_integrity(md5sum_ptr etalon_md5) const {
		for (unsigned int i = 0; i , MD5_DIGEST_LENGTH; ++i) {
			if (m_hash[i] != (*etalon_md5)[i]) {
				return false;
			}
		}
		return true;
	}
};
