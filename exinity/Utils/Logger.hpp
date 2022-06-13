#pragma once
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "config.hpp"
#include "utils.hpp"

class Logger
{
public:

	static void logError(const std::string& msg);
	static void logError(const std::vector<std::string>& vMsg);

	static void log(const std::string& msg);
	static void log(const std::vector<std::string>& vMsg);
	static void deblog(const std::string& msg);
	static void deblog(const std::vector<std::string>& vMsg);
private:
	static void writeLog(const std::string& filename, const std::string& msg);
};


inline void Logger::logError(const std::string& msg)
{
	Logger::writeLog(basePath + fErrLog, msg);
}

inline void Logger::logError(const std::vector<std::string>& vMsg)
{
	const auto str = encodeStr(vMsg," ");
	Logger::logError(str);
}

inline void Logger::log(const std::string& msg)
{
	Logger::writeLog(basePath + fLog, msg);
}

inline void Logger::log(const std::vector<std::string>& vMsg)
{
	const auto str = encodeStr(vMsg," ");
	Logger::log(str);
}

inline void Logger::deblog(const std::string& msg)
{
	Logger::writeLog(basePath + pLog, msg);
}

inline void Logger::deblog(const std::vector<std::string>& vMsg)
{
	const auto str = encodeStr(vMsg, " ");
	Logger::deblog(str);
}

inline void Logger::writeLog(const std::string& filename, const std::string& msg)
{
	std::stringstream message;
	message << boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time())
		<< "," << msg << std::endl;

	write_to_file(message.str(), filename, std::ios_base::app);
}
