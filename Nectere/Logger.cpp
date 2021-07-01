#include "Logger.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Configuration.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	Logger Logger::out("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}");
	Logger Logger::ScriptEngine::init("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}");
	Logger Logger::ScriptEngine::parsing("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}");
	Logger Logger::ScriptEngine::compilation("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}");
	Logger Logger::ScriptEngine::runtime("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}");

	Logger::Logger() : m_Mute(false) {}
	Logger::Logger(const std::string &format) : m_Mute(false), m_Format(format){}

	void Logger::SetFormat(const std::string &format) { m_Format = format; }
	void Logger::Mute() { m_Mute = true; }
	void Logger::Unmute() { m_Mute = false; }

	void Logger::Log(LogType logType, const std::string &str) const
	{
		if (logType == LogType::Verbose && Configuration::Is("Verbose", false))
			return;
		std::string message = str;
		std::string log = m_Format;
		StringUtils::ReplaceAll(log, "{msg}", str);
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		size_t milliSeconds = (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm *localTime = std::localtime(&time);
		StringUtils::ReplaceAll(log, "{y}", std::to_string(localTime->tm_year + 1900));
		//month
		if (localTime->tm_mon >= 9)
			StringUtils::ReplaceAll(log, "{M}", std::to_string(localTime->tm_mon + 1));
		else
			StringUtils::ReplaceAll(log, "{M}", "0" + std::to_string(localTime->tm_mon + 1));
		//day
		if (localTime->tm_mday >= 10)
			StringUtils::ReplaceAll(log, "{d}", std::to_string(localTime->tm_mday));
		else
			StringUtils::ReplaceAll(log, "{d}", "0" + std::to_string(localTime->tm_mday));
		//hour
		if (localTime->tm_hour >= 10)
			StringUtils::ReplaceAll(log, "{h}", std::to_string(localTime->tm_hour));
		else
			StringUtils::ReplaceAll(log, "{h}", "0" + std::to_string(localTime->tm_hour));
		//minute
		if (localTime->tm_min >= 10)
			StringUtils::ReplaceAll(log, "{m}", std::to_string(localTime->tm_min));
		else
			StringUtils::ReplaceAll(log, "{m}", "0" + std::to_string(localTime->tm_min));
		//second
		if (localTime->tm_sec >= 10)
			StringUtils::ReplaceAll(log, "{s}", std::to_string(localTime->tm_sec));
		else
			StringUtils::ReplaceAll(log, "{s}", "0" + std::to_string(localTime->tm_sec));
		//millisecond
		if (milliSeconds >= 100)
			StringUtils::ReplaceAll(log, "{ms}", std::to_string(milliSeconds));
		else if (milliSeconds >= 10)
			StringUtils::ReplaceAll(log, "{ms}", "0" + std::to_string(milliSeconds));
		else
			StringUtils::ReplaceAll(log, "{ms}", "00" + std::to_string(milliSeconds));
		std::unique_lock<std::mutex> lock(m_Mutex);
		switch (logType)
		{
		case Nectere::LogType::Standard:
		{
			StringUtils::ReplaceAll(log, "{log_type}", "");
			std::cout << log << std::endl;
			LogFile(log, "", localTime);
			break;
		}
		case Nectere::LogType::Warning:
		{
			StringUtils::ReplaceAll(log, "{log_type}", "Warning: ");
			std::cerr << log << std::endl;
			LogFile(log, "-warn", localTime);
			break;
		}
		case Nectere::LogType::Error:
		{
			StringUtils::ReplaceAll(log, "{log_type}", "Error: ");
			std::cerr << log << std::endl;
			LogFile(log, "-err", localTime);
			break;
		}
		case LogType::Verbose:
		{
			StringUtils::ReplaceAll(log, "{log_type}", "");
			std::cout << log << std::endl;
			break;
		}
		case Nectere::LogType::Debug:
		{
			StringUtils::ReplaceAll(log, "{log_type}", "Debug: ");
			std::cout << log << std::endl;
			LogFile(log, "-deb", localTime);
			break;
		}
		}
	}

	void Logger::LogFile(const std::string &log, const std::string &suffix, std::tm *localTime) const
	{
		if (Configuration::Is("LogFile", true))
		{
			std::string path = Configuration::Get<std::string>("LogPath");
			std::stringstream filename;
			filename << "Nectere-";
			filename << std::to_string(localTime->tm_year + 1900);
			filename << '-';
			if (localTime->tm_mon < 9)
				filename << "0";
			filename << std::to_string(localTime->tm_mon + 1);
			filename << '-';
			if (localTime->tm_mday < 10)
				filename << "0";
			filename << std::to_string(localTime->tm_mday);
			filename << suffix;
			filename << ".log";
			std::filesystem::path logFilePath = std::filesystem::absolute(path + filename.str());
			std::filesystem::create_directories(logFilePath.parent_path());
			std::ofstream outfile;
			outfile.open(logFilePath, std::ios_base::app); // append instead of overwrite
			outfile << log << std::endl;
			outfile.close();
		}
	}
}