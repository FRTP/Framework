#ifndef CPARSER_HPP
#define CPARSER_HPP

#include <string>
#include <vector>

class CParser {
	private:
		std::vector<std::string> m_args;
		std::string m_logname;
		std::string m_configname;
	public:
		CParser(int argc, char** argv) : m_logname("/var/frtp/frtp.log"), m_configname("~/.config/frtp/main.conf") {
			m_args.reserve(argc);
			for(int i = 1; i < argc; ++i)
				m_args.push_back(argv[i]);
		}

		void parse() {
			for(size_t i = 0; i < m_args.size(); ++i) {
				if ((m_args[i] == "--log" || m_args[i] == "-l") && (i + 1) < m_args.size()) {
					m_logname = m_args[++i];
				}
				else if ((m_args[i] == "--config" || m_args[i] == "-c") && (i + 1) < m_args.size()) {
					m_configname = m_args[++i];
				}
			}
		}
		
		std::string logname() const {
			return m_logname;	
		}

		std::string configname() const {
			return m_configname;
		}

};

#endif //CPARSER_HPP
