#pragma once
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "config.hpp"
#include "Utils.hpp"
#include <filesystem>

class Logger
{
public:

	static void log(const std::string& msg);
	static void log(const std::vector<std::string>& vMsg);
	static void dumplog(const std::string& msg);
	static void dumplog(const std::vector<std::string>& vMsg);
	static void initializeLog();
private:
	static void writeLog(const std::string& filename, const std::string& msg);
};

inline void Logger::initializeLog()
{
	if (!std::filesystem::exists(basePath + dumpLog)) {
		const std::string first_mes_event = "#time,event,value\n";
		write_to_file(first_mes_event, basePath + dumpLog, std::ios_base::app);
	}
	//else {
	//	std::filesystem::remove(basePath + dumpLog);
	//}

	if (!std::filesystem::exists(basePath + servLog)) {
		const std::string first_mes_event = "#time,event,value,result\n";
		write_to_file(first_mes_event, basePath + servLog, std::ios_base::app);
	}
	//else {
	//	std::filesystem::remove(basePath + servLog);
	//}

	return;
}

inline void Logger::log(const std::string& msg)
{
	Logger::writeLog(basePath + servLog, msg);
}

inline void Logger::log(const std::vector<std::string>& vMsg)
{
	const auto str = encodeStr(vMsg,",");
	Logger::log(str);
}

inline void Logger::dumplog(const std::string& msg)
{
	Logger::writeLog(basePath + dumpLog, msg);
}

inline void Logger::dumplog(const std::vector<std::string>& vMsg)
{
	const auto str = encodeStr(vMsg, ",");
	Logger::dumplog(str);
}

inline void Logger::writeLog(const std::string& filename, const std::string& msg)
{
	std::stringstream message;
	message << boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time())
		<< "," << msg << std::endl;

	write_to_file(message.str(), filename, std::ios_base::app);
}
