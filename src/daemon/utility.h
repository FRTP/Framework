#ifndef UTILITY_H
#define UTILITY_H

#include <boost/array.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <list>
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
		INTERNAL_ERROR,
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
			static constexpr const char* MESSAGE_ENDING = "\r\n\r\n";
			CMessage();
			CMessage(ECommand cmd, EDataType datatype, const std::vector<char>& data);
			void to_streambuf(boost::asio::streambuf& buffer) const;
			EError from_streambuf(boost::asio::streambuf& buffer);
			ECommand command() const;
			EDataType datatype() const;
			std::vector<char>& data();
			std::vector<char>::iterator data_begin();
			std::vector<char>::iterator data_end();
	};

	class ICommand {
		public:
			virtual ECommand type() const = 0;
			virtual EError invoke(CContext* context, EDataType datatype) = 0;
			virtual ~ICommand() {}
	};

	class IAbstractCommandCreator {
		public:
			virtual ICommand* create(const std::list<std::string>& args) const = 0;
			virtual ~IAbstractCommandCreator() {}
	};

	template<class T>
	class CCommandCreator : public IAbstractCommandCreator {
		public:
			virtual ICommand* create(const std::list<std::string>& args) const {
				return new T(args);
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
				return NULL;
			}

			static ICommand* create(const CMessage& msg) {
				auto it = m_factory.find(_cmd_to_string(msg.command()));
				if (it != m_factory.end()) {
					return it->second->create(msg);
				}
				return NULL;
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
