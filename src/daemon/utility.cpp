#include "utility.h"

namespace utility {
	std::map<std::string, IAbstractCommandCreator*> CCommandFactory::m_factory;
	std::string CSettings::m_working_dir;
	std::string CSettings::m_data_dir;

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
			case EError::INTERNAL_ERROR:
				res = "Internal error";
				break;
			case EError::CORRUPTED_MESSAGE:
				res = "Corrupted message";
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
				return std::string("shares/");
			case EDataType::TWITTER:
				return std::string("twitter/");
			default:
				return std::string("");
		}
	}

	std::string get_full_path(EDataType type, const std::string& filename) {
		return CSettings::data_dir() + get_data_type_dir(type) + filename;
	}

	md5sum_ptr calculate_md5(const std::string& full_path) {
		md5sum_ptr md5(new md5sum);
		md5->resize(MD5_DIGEST_LENGTH);
		std::ifstream in(full_path);

		in.clear();
		in.seekg(0, std::ios::end);
		int size = in.tellg();
		if (size < 0) {
			return nullptr;
		}
		data_t content;
		content.resize(size);
		in.seekg(0);
		in.read(reinterpret_cast<char*>(&content[0]), size);

		MD5(content.data(), content.size(), md5->data());
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

	void str_to_data_t(const std::string& input, data_t& output) {
		output.resize(input.size());
		for (unsigned int i = 0; i < input.size(); ++i) {
			output[i] = static_cast<unsigned char>(output[i]);
		}
	}

	void CSettings::set_working_dir(const std::string& working_dir) {
		m_working_dir = working_dir;
	}

	void CSettings::set_data_dir(const std::string& data_dir) {
		m_data_dir = data_dir;
	}

	std::string CSettings::working_dir() {
		return m_working_dir;
	}

	std::string CSettings::data_dir(bool relative) {
		std::string res = m_data_dir;
		if (!relative) {
			res = m_working_dir + res;
		}
		return res;
	}

	CMessage::CMessage() {
	}

	CMessage::CMessage(ECommand cmd, EDataType datatype, const data_t& data)
		: m_cmd(cmd), m_datatype(datatype), m_data(data) {
		_calculate_hash();
	}

	CMessage::CMessage(ECommand cmd, EDataType datatype, const std::vector<char>& data)
		: m_cmd(cmd), m_datatype(datatype) {
		m_data.reserve(data.size());
		for (char i : data) {
			m_data.push_back(static_cast<unsigned char>(i));
		}
		_calculate_hash();
	}

	void CMessage::_calculate_hash() {
		//dirty hack to calculate md5 from all packet fields together without excessive copying of the data field
		m_data.reserve(m_data.size() + 2);
		m_data.push_back(static_cast<char>(m_cmd));
		m_data.push_back(static_cast<char>(m_datatype));

		m_hash.resize(MD5_DIGEST_LENGTH);
		MD5(&m_data[0], m_data.size(), m_hash.data());

		//clean m_data after the dirty hack
		m_data.pop_back();
		m_data.pop_back();
		m_data.shrink_to_fit();
	}

	void CMessage::to_streambuf(boost::asio::streambuf& buffer) const {
		std::ostream out(&buffer);
		out << static_cast<int>(m_cmd) << " " << static_cast<int>(m_datatype) << " " << m_data.size() << " ";
		for (auto i : m_data) {
			out << i;
		}
		out << " ";
		for (auto i : m_hash) {
			out << i;
		}
		out << " " << MESSAGE_ENDING;
	}

	EError CMessage::from_streambuf(boost::asio::streambuf& buffer) {
		m_data.clear();
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
		unsigned int datasize = 0;
		in >> datasize;
		in.ignore();
		m_data.resize(datasize);

		in.read(reinterpret_cast<char*>(&m_data[0]), datasize);
		_calculate_hash();

		md5sum standart_hash;
		standart_hash.resize(MD5_DIGEST_LENGTH);
		in.ignore();
		in.read(reinterpret_cast<char*>(&standart_hash[0]), MD5_DIGEST_LENGTH);
		for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
			if (m_hash[i] != standart_hash[i]) {
				return EError::CORRUPTED_MESSAGE;
			}
		}
		return EError::OK;
	}

	ECommand CMessage::command() const {
		return m_cmd;
	}

	EDataType CMessage::datatype() const {
		return m_datatype;
	}

	const data_t& CMessage::data() const {
		return m_data;
	}

	data_t::const_iterator CMessage::data_begin() const {
		return m_data.cbegin();
	}

	data_t::const_iterator CMessage::data_end() const {
		return m_data.cend();
	}
};
