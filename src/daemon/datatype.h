#ifndef DATATYPE_H
#define DATATYPE_H

#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "utility.h"

namespace logging = boost::log;
using namespace utility;

namespace datatypes {
	class IDataType {
		public:
			virtual EError get_data(std::vector<char>& output) const = 0;
			virtual EError write_data(const std::vector<char>& input) const = 0;
			virtual bool success() const = 0;
			virtual ~IDataType() {}
	};

	class CDataTypeShares : public IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			static constexpr const char* m_data_dir = "shares";
			std::string m_filename;
			bool m_success;
		public:
			CDataTypeShares(const std::list<std::string>& args);
			virtual EError get_data(std::vector<char>& output) const;
			virtual EError write_data(const std::vector<char>& input) const;
			virtual bool success() const;
			virtual ~CDataTypeShares() {}
	};

	class CDataTypeTwitter : public IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			static constexpr const char* m_data_dir = "twitter";
			std::string m_filename;
			bool m_success;
		public:
			CDataTypeTwitter(const std::list<std::string>& args);
			virtual EError get_data(std::vector<char>& output) const;
			virtual EError write_data(const std::vector<char>& input) const;
			virtual bool success() const;
			virtual ~CDataTypeTwitter() {}
	};

	class IAbstractDataTypeCreator {
		public:
			virtual IDataType* create(const std::list<std::string>& args) const = 0;
			virtual ~IAbstractDataTypeCreator() {}
	};

	template<class DataType>
	class CDataTypeCreator : public IAbstractDataTypeCreator {
		public:
			virtual IDataType* create(const std::list<std::string>& args) const {
				return new DataType(args);
			}
			virtual ~CDataTypeCreator() {}
	};

	class CDataTypeFactory {
		private:
			typedef std::map<EDataType, IAbstractDataTypeCreator*> types_map;
			static types_map m_types;
		public:
			template<class DataType>
			static void register_type(EDataType type) {
				auto it = m_types.find(type);
				if (it == m_types.end()) {
					m_types[type] = new CDataTypeCreator<DataType>();  
				}
			}
			static IDataType* create(EDataType type, const std::list<std::string>& args) {
				auto it = m_types.find(type);
				if (it != m_types.end()) {
					return it->second->create(args);
				}
				return 0;
			}
			~CDataTypeFactory() {
				for (auto it = m_types.begin(); it != m_types.end(); ++it) {
					if (it->second) {
						delete it->second;
					}
				}
			}
	};
}

#endif //DATATYPE_H
