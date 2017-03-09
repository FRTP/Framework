#ifndef DATATYPE_H
#define DATATYPE_H

#include <boost/shared_ptr.hpp>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "clog.hpp"
#include "utility.h"

using namespace utility;

namespace datatypes {
	enum EDataType {
		SHARES,
		TWITTER,
		MAX_VAL = TWITTER
	};

	class IDataType {
		public:
			virtual EError get_data(std::vector<char>& output, boost::shared_prt<CLog> log) = 0;
			virtual ~IDataType() {}
	};

	class CDataTypeShares : public IDataType {
		private:
			static constexpr EXPECTED_ARGS_NUM = 1;
			static std::string m_data_dir = "shares";
			std::string m_filename;
		public:
			CDataTypeShares();
			virtual EError get_data(std::vector<char>& output, boost::shared_ptr<CLog> log);
			virtual ~CDataTypeShares() {}
	};

	class CDataTypeTwitter : public IDataType {
		private:
			static constexpr EXPECTED_ARGS_NUM = 1;
			static std::string m_data_dir = "twitter";
			std::string m_filename;
		public:
			CDataTypeTwitter(const std::list<std::string>& args);
			virtual EError get_data(std::vector<char>& output, boost::shared_ptr<CLog> log);
			virtual ~CDataTypeTwitter() {}
	};

	template<class DataType>
	class CDataTypeCreator {
		public:
			IDataType* create(const std::list<std::string>& args) const {
				return new DataType(args);
			}
			~CDataTypeCreator() {}
	};

	class CDataTypeFactory {
		private:
			typedef std::map<EDataType, CDataTypeCreator*> types_map;
			static types_map m_types;
		public:
			template<class DataType>
			static void register_type(EDataType type) {
				auto it = m_types.find(type);
				if (it == m_types.end()) {
					m_types[type] = new CDataTypeCreator<DataType>();  
				}
			}
			static IDataType* create(EDataType type) {
				auto it = types_map.find(type);
				if (it != types_map.end()) {
					return it->second->create();
				}
				return 0;
			}
			~CDataTypeFactory() {
				for (auto it = types_map.begin(); it != types_map.end(); ++it) {
					if (it->second) {
						delete it->second;
					}
				}
			}
	};
}

#endif //DATATYPE_H
