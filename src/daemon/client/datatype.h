#ifndef DATATYPE_H
#define DATATYPE_H

#include <boost/shared_ptr.hpp>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../idatatype.h"
#include "../utility.h"

using namespace utility;

namespace datatypes {
	EError read_binary(data_t& output, const std::string& full_path, bool append);
	EError write_binary(const std::string& full_path, data_t::const_iterator begin,
			    data_t::const_iterator end);

	class CDataTypeShares : public : IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			std::string m_filename;
			std::string m_source;
			bool m_success;
		public:
			CDataTypeShares(const std::list<std::string>& args);
			virtual EError get_data(data_t& output, bool append) const;
			virtual EError write_data(data_t::const_iterator begin,
						  data_t::const_iterator end) const;
			virtual bool success() const;
			virtual std::string full_path() const;
			virtual std::string path() const;
			virtual ~CDataTypeShares() {}
	};

	class CDataTypeTwitter : public : IDataType {
		private:
			static constexpr int EXPECTED_ARGS_NUM = 1;
			std::string m_filename;
			bool m_success;
		public:
			CDataTypeTwitter(const std::list<std::string>& args);
			virtual EError get_data(data_t& output, bool append) const;
			virtual EError write_data(data_t::const_iterator begin,
						  data_t::const_iterator end) const;
			virtual bool success() const;
			virtual std::string full_path() const;
			virtual std::string path() const;
			virtual ~CDataTypeTwitter() {}
	};
}

#endif //DATATYPE_H
