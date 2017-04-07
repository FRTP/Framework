#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <string>

class ExError : public std::exception {
	private:
		std::string m_message;
		std::string m_details;
	public:
		ExError() : m_message(""), m_details("") {}
		explicit ExError(const std::string& message) : m_message(message), m_details("") {}
		explicit ExError(const std::string& message, const std::string& details)
			: m_message(message), m_details(details) {}
		virtual const char* what() const noexcept {
			std::string res = m_message + " in " + m_details;
			return res.c_str();
		}
		virtual const std::string& get_message() const {
			return m_message;
		}
		virtual const std::string& get_details() const {
			return m_details;
		}
};

class ExInvalidArgs : public ExError {
	public:
		ExInvalidArgs() : ExError() {}
		explicit ExInvalidArgs(const std::string& message) : ExError(message) {}
		explicit ExInvalidArgs(const std::string& message, const std::string& details)
			: ExError(message, details) {}
};

class ExConnectionProblem : public ExError {
	public:
		ExConnectionProblem() : ExError() {}
		explicit ExConnectionProblem(const std::string& message) : ExError(message) {}
		explicit ExConnectionProblem(const std::string& message, const std::string& details)
			: ExError(message, details) {}
};

class ExSocketProblem : public ExError {
	public:
		ExSocketProblem() : ExError() {}
		explicit ExSocketProblem(const std::string& message) : ExError(message) {}
		explicit ExSocketProblem(const std::string& message, const std::string& details)
			: ExError(message, details) {}
};

class ExUnknownDataType : public ExError {
	public:
		ExUnknownDataType() : ExError() {}
		explicit ExUnknownDataType(const std::string& message) : ExError(message) {}
		explicit ExUnknownDataType(const std::string& message, const std::string& details)
			: ExError(message, details) {}
};

class ExNoFile : public ExError {
	public:
		ExNoFile() : ExError() {}
		explicit ExNoFile(const std::string& message) : ExError(message) {}
		explicit ExNoFile(const std::string& message, const std::string& details)
			: ExError(message, details) {}
};

#endif //EXCEPTION_HPP
