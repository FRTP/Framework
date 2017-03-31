#ifndef DATATYPE_H
#define DATATYPE_H

#include <boost/shared_ptr.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
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
	class CDataTypeShares : public IDataType {
		private:
			enum class ESource {
				YAHOO,
				BCS,
				FAST,
				NO_SOURCE
			};

			static constexpr int EXPECTED_ARGS_NUM = 2;
			std::string m_filename;
			ESource m_source;
			bool m_success;

			void _arg_to_esource(const std::string& input);

			EError _from_bcs(data_t& output, bool append) const;
			EError _from_yahoo(data_t& output, bool append) const;
			EError _from_fast(data_t& output. bool append) const;
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

	class CDataTypeTwitter : public IDataType {
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
