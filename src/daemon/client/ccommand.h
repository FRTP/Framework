#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <boost/asio.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/python.hpp>
#include <fstream>
#include <map>
#include <list>
#include <string>
#include <vector>

#include "ccontext.hpp"
#include "../datatype.h"
#include "exception.hpp"
#include "../utility.hpp"

namespace fs = boost::filesystem;
using namespace boost::asio;
using namespace utility;

class ICommand {
	public:
		virtual ECommand type() const = 0;
		virtual EError invoke(CContext* context, EDataType datatype) = 0;
		virtual ~ICommand() {}
};

class CCmdGetFile : public ICommand {
	private:
		static constexpr EXPECTED_ARGS_NUM = 3;
		std::string m_filename;
		std::string m_newfilename;
		streambuf m_buffer;
		bool m_force_update;
	public:
		explicit CCmdGetFile(const std::list<std::string>& args);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype)
		~CCmdGetFile() {}
};

class CCmdGetMD5 : public ICommand {
	private:
		static constexpr EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
		md5sum_ptr m_hash;
	public:
		explicit CCmdGetMD5(const std::list<std::string>& args);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		md5sum_ptr hash() const;
		~CCmdGetMD5() {}
};

class CCmdUploadFile : public ICommand {
	private:
		static constexpr EXPECTED_ARGS_NUM = 1;
		std::string m_filename;
	public:
		explicit CCmdUploadFile(const std::list<std::string>& args);
		virtual ECommand type() const;
		virtual EError invoke(CContext* context, EDataType datatype);
		~CCmdUploadFile() {}
};

template<class T>
class CCommandCreator {
	public:
		virtual ICommand* create(const std::list<std::string>& args) const {
			return new T(args);
		}
		virtual ~CCommandCreator() {}
};

class CCommandFactory {
	private:
		typedef std::map<std::string, CCommandCreator*> factory_map;
		factory_map m_factory;
	public:
		template<class T>
		static void add(const std::string& id) {
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
			return 0;
		}
		~CCommandFactory() {
			for (auto it = m_factory.begin(); it != m_factory.end(); ++it) {
				if (it->second) {
					delete it->second;
				}
			}
		}
};

#endif //CCOMMAND_H
