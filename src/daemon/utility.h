#ifndef UTILITY_H
#define UTILITY_H

#include <boost/array.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <vector>

class CContext;

namespace utility {
	typedef std::vector<unsigned char> hash_t;
	typedef boost::shared_ptr<hash_t> hash_ptr;
	typedef hash_t md5sum;
	typedef hash_t sha512;
	typedef boost::shared_ptr<md5sum> md5sum_ptr;
	typedef boost::shared_ptr<sha512> sha512_ptr;
	typedef std::vector<unsigned char> data_t;

	enum class ECommand {
		AUTHORIZE,
		REGISTER,
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
		INTERNAL_ERROR,
		CORRUPTED_MESSAGE,
		DB_ERROR,
		AUTH_ERROR,
		INVALID_LOGIN,
		UNKNOWN_ERROR,
		MAX_VAL = UNKNOWN_ERROR
	};

	enum class EDataType {
		ASSETS,
		TWITTER,
		ACCOUNT,
		ERROR_CODE,
		MAX_VAL = ERROR_CODE
	};

	class CMessage;

	std::string get_text_error(EError error);
	std::string get_data_type_dir(EDataType type); 
	std::string get_full_path(EDataType type, const std::string& filename);
	sha512_ptr encrypt_string(const std::string& input);
	md5sum_ptr calculate_md5(const std::string& full_path);
	std::string hash_to_str(hash_ptr hash);
	void str_to_data_t(const std::string& input, data_t& output, bool append);
	EError check_message(const CMessage& msg);

	class CSettings {
		private:
			static std::string m_working_dir;
			static std::string m_data_dir;
		public:
			static void set_working_dir(const std::string& working_dir);
			static void set_data_dir(const std::string& data_dir);
			static std::string get_working_dir();
			static std::string get_data_dir(bool relative = false);
	};

	class CMessage {
		private:
			ECommand m_cmd;
			EDataType m_datatype;
			data_t m_data;
			md5sum m_hash;

			void _calculate_hash();
		public:
			static constexpr const char* MESSAGE_ENDING = "\r\n\r\n";
			CMessage();
			CMessage(ECommand cmd, EDataType datatype, const data_t& data);
			CMessage(ECommand cmd, EDataType datatype, const std::vector<char>& data);
			void to_streambuf(boost::asio::streambuf& buffer) const;
			EError from_streambuf(boost::asio::streambuf& buffer);
			ECommand get_command() const;
			EDataType get_datatype() const;
			const data_t& data() const;
			data_t::const_iterator data_begin() const;
			data_t::const_iterator data_end() const;
	};

	class ICommand {
		public:
			virtual ECommand type() const = 0;
			virtual EError invoke(boost::shared_ptr<CContext>& context, EDataType datatype) = 0;
			virtual ~ICommand() {}
	};

	class IAbstractCommandCreator {
		public:
			virtual ICommand* create(const std::list<std::string>& args) const = 0;
			virtual ICommand* create(const CMessage& msg) const = 0;
			virtual ~IAbstractCommandCreator() {}
	};

	template<class T>
	class CCommandCreator : public IAbstractCommandCreator {
		public:
			virtual ICommand* create(const std::list<std::string>& args) const {
				return new T(args);
			}

			virtual ICommand* create(const CMessage& msg) const {
				return new T(msg);
			}

			virtual ~CCommandCreator() {}
	};

	class CCommandFactory {
		private:
			typedef std::map<std::string, IAbstractCommandCreator*> factory_map;
			static factory_map m_factory;

			static std::string _cmd_to_string(ECommand cmd) {
				switch (cmd) {
					case ECommand::FEEDBACK:
						return "Feedback";
					case ECommand::UPLOAD_FILE:
						return "UploadFile";
					case ECommand::GET_MD5:
						return "GetMD5";
					case ECommand::GET_FILE:
						return "GetFile";
					case ECommand::AUTHORIZE:
						return "Authorize";
					case ECommand::REGISTER:
						return "Register";
					default:
						return "";
				}
			}
		public:
			template<class T>
			static void add(const std::string& id) {
				auto it = m_factory.find(id);
				if (it == m_factory.end()) {
					m_factory[id] = new CCommandCreator<T>();
				}
			}

			template<class T>
			static void add(ECommand cmd) {
				std::string id = _cmd_to_string(cmd);
				auto it = m_factory.find(id);
				if (it == m_factory.end()) {
					m_factory[id] = new CCommandCreator<T>();
				}
			}

			static ICommand* create(const std::string& id, boost::python::list args) {
				auto it = m_factory.find(id);
				if (it != m_factory.end()) {
					std::list<std::string> arguments;
					for (int i = 0; i < len(args); ++i) {
						arguments.push_back(boost::python::extract<std::string>(args[i]));
					}
					return it->second->create(arguments);
				}
				return nullptr;
			}

			static ICommand* create(const CMessage& msg) {
				auto it = m_factory.find(_cmd_to_string(msg.get_command()));
				if (it != m_factory.end()) {
					return it->second->create(msg);
				}
				return nullptr;
			}

			~CCommandFactory() {
				for (auto it = m_factory.begin(); it != m_factory.end(); ++it) {
					if (it->second) {
						delete it->second;
					}
				}
			}
	};
};

#endif //UTILITY_H
