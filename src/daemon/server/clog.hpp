#ifndef CLOG_HPP
#define CLOG_HPP

#include <fstream>
#include <string>

class CLog {
	private:
		std::ofstream m_log;
	public:
		explicit CLog(const std::string& logname) : m_log(logname) {}
		~CLog()  {
			m_log.close();
		}

		void write(const std::string& info) {
			m_log << info << std::endl;
		}
};

#endif //CLOG_HPP
