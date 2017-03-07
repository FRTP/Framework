#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <boost/asio.hpp>
#include <boost/python.hpp>
#include <fstream>
#include <map>
#include <list>
#include <string>

#include "ccontext.hpp"
#include "exception.hpp"

using namespace boost::asio;

enum ECommand {
	GET_FILE,
	GET_MD5
};

class ICommand {
	public:
		virtual ECommand type() const = 0;
		virtual void invoke(CContext* context, int& error) = 0;
		virtual ~ICommand() {}
};

class CCmdGetFile : public ICommand {
	private:
		std::string m_filename;
		std::string m_newfilename;
		streambuf m_buffer;
	public:
		explicit CCmdGetFile(const std::list<std::string>& args);
		virtual ECommand type() const;
		virtual void invoke(CContext* context, int& error);
		~CCmdGetFile() {}
};

class CCmdGetMD5 : public ICommand {
	private:
		std::string m_filename;
		const unsigned char* m_hash;
		int m_hash_size;
	public:
		explicit CCmdGetMD5(const std::list<std::string>& args);
		virtual ECommand type() const;
		virtual void invoke(CContext* context, int& error);
		const unsigned char* hash(int& size);
		~CCmdGetMD5() {}
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
		typedef std::map<std::string, IAbstractCommandCreator*>::iterator factory_iter;
		static std::map<std::string, IAbstractCommandCreator*> m_factory;
	public:
		template<class T>
		static void add(const std::string& id) {
			factory_iter it = m_factory.find(id);
			if (it == m_factory.end()) {
				m_factory[id] = new CCommandCreator<T>();
			}
		}
		static ICommand* create(const std::string& id, boost::python::list args) {
			factory_iter it = m_factory.find(id);
			if (it != m_factory.end()) {
				std::list<std::string> arguments;
				for (int i = 0; i < len(args); ++i) {
					arguments.push_back(boost::python::extract<std::string>(args[i]));
				}
				return it->second->create(arguments);
			}
			return 0;
		}
		~CCommandFactory() {
			for (factory_iter it = m_factory.begin(); it != m_factory.end(); ++it) {
				if (it->second) {
					delete it->second;
				}
			}
		}
};

#endif //CCOMMAND_H
