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
	class CDataTypeAssets : public IDataType {
		private:
			enum class ESource {
				YAHOO,
				BCS,
				FAST,
				NO_SOURCE
			};

			static constexpr int EXPECTED_ARGS_NUM = 1;
			std::string m_filename;
			std::string m_source;
			bool m_success;

			ESource _arg_to_esource(const std::string& input) const;

			utility::EError _from_bcs(utility::data_t& output, bool append) const;
			utility::EError _from_yahoo(utility::data_t& output, bool append) const;
			utility::EError _from_fast(utility::data_t& output, bool append) const;
		public:
			CDataTypeAssets(const std::list<std::string>& args);
			virtual utility::EError get_data(utility::data_t& output, bool append = false) const;
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
			CDataTypeTwitter(const std::list<std::string>& args);
			virtual utility::EError get_data(utility::data_t& output, bool append = false) const;
			virtual utility::EError write_data(utility::data_t::const_iterator begin,
							   utility::data_t::const_iterator end) const;
			virtual bool is_success() const;
			virtual std::string get_full_path() const;
			virtual std::string get_path() const;
			virtual ~CDataTypeTwitter() {}
	};
}

#endif //DATATYPE_H
