#ifndef IDATATYPE_H
#define IDATATYPE_H

#include <boost/shared_ptr.hpp>
#include <list>
#include <map>
#include <string>

#include "utility.h"

using namespace utility;

class IDataType {
	public:
		virtual EError get_data(data_t& output, bool append) const = 0;
		virtual EError write_data(data_t::const_iterator begin,
					  data_t::const_iterator end) const = 0;
		virtual bool success() const = 0;
		virtual std::string full_path() const = 0;
		virtual std::string path() const = 0;
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
		typedef std::map<EDataType, boost::shared_ptr<IAbstractDataTypeCreator>> types_map;
		static types_map m_types;
	public:
		template<class DataType>
		static void register_type(EDataType type) {
			auto it = m_types.find(type);
			if (it == m_types.end()) {
				m_types[type] = boost::shared_ptr<IAbstractDataTypeCreator>(new CDataTypeCreator<DataType>());
			}
		}
		static boost::shared_ptr<IDataType> create(EDataType type, const std::list<std::string>& args) {
			auto it = m_types.find(type);
			if (it != m_types.end()) {
				return it->second->create(args);
			}
			return 0;
		}
		~CDataTypeFactory() {}
};

#endif //IDATATYPE_H
