#ifndef DATATYPE_H
#define DATATYPE_H

#include <boost/shared_ptr.hpp>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "utility.h"

namespace datatypes {
	class IDataType {
		public:
			virtual utility::EError get_data(utility::data_t& output) const = 0;
			virtual utility::EError append_data(utility::data_t& output) const = 0;
			virtual utility::EError write_data(utility::data_t::const_iterator begin,
							   utility::data_t::const_iterator end) const = 0;
			virtual bool is_success() const = 0;
			virtual std::string get_full_path() const = 0;
			virtual std::string get_path() const = 0;
			virtual ~IDataType() {}
	};

	class CDataTypeAssets : public IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			std::string m_filename;
			bool m_success;
		public:
			explicit CDataTypeAssets(const std::list<std::string>& args);
			virtual utility::EError get_data(utility::data_t& output) const;
			virtual utility::EError append_data(utility::data_t& output) const;
			virtual utility::EError write_data(utility::data_t::const_iterator begin,
							   utility::data_t::const_iterator end) const;
			virtual bool is_success() const;
			virtual std::string get_full_path() const;
			virtual std::string get_path() const;
			virtual ~CDataTypeAssets() {}
	};

	class CDataTypeTwitter : public IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			std::string m_filename;
			bool m_success;
		public:
			explicit CDataTypeTwitter(const std::list<std::string>& args);
			virtual utility::EError get_data(utility::data_t& output) const;
			virtual utility::EError append_data(utility::data_t& output) const;
			virtual utility::EError write_data(utility::data_t::const_iterator begin,
							   utility::data_t::const_iterator end) const;
			virtual bool is_success() const;
			virtual std::string get_full_path() const;
			virtual std::string get_path() const;
			virtual ~CDataTypeTwitter() {}
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
				return nullptr;
			}
			~CDataTypeFactory() {}
	};
}

#endif //DATATYPE_H

