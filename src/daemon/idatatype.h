#ifndef IDATATYPE_H
#define IDATATYPE_H

#include <boost/shared_ptr.hpp>
#include <list>
#include <map>
#include <string>

#include "utility.h"

namespace datatypes {
	class IDataType {
		public:
			virtual utility::EError get_data(utility::data_t& output, bool append = false) const = 0;
			virtual utility::EError write_data(utility::data_t::const_iterator begin,
							   utility::data_t::const_iterator end) const = 0;
			virtual bool is_success() const = 0;
			virtual std::string get_full_path() const = 0;
			virtual std::string get_path() const = 0;
			virtual ~IDataType() {}
	};

	class IAbstractDataTypeCreator {
		public:
			virtual boost::shared_ptr<IDataType> create(const std::list<std::string>& args) const = 0;
			virtual ~IAbstractDataTypeCreator() {}
	};

	template<class DataType>
	class CDataTypeCreator : public IAbstractDataTypeCreator {
		public:
			virtual boost::shared_ptr<IDataType> create(const std::list<std::string>& args) const {
				return boost::shared_ptr<IDataType>(new DataType(args));
			}
			virtual ~CDataTypeCreator() {}
	};

	class CDataTypeFactory {
		private:
			typedef std::map<utility::EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> types_map;
			static types_map m_types;
		public:
			template<class DataType>
			static void register_type(utility::EDataType type) {
				auto it = m_types.find(type);
				if (it == m_types.end()) {
					m_types[type] = boost::shared_ptr<IAbstractDataTypeCreator>(new CDataTypeCreator<DataType>());
				}
			}
			static boost::shared_ptr<IDataType> create(utility::EDataType type, const std::list<std::string>& args) {
				auto it = m_types.find(type);
				if (it != m_types.end()) {
					return it->second->create(args);
				}
				return 0;
			}
			~CDataTypeFactory() {}
	};
}

#endif //IDATATYPE_H
