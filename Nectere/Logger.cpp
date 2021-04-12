#include "Logger.hpp"

#include <chrono>
#include <iostream>

namespace Nectere
{
	Logger Logger::err("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] ERROR: {msg}", [](const std::string &str) {std::cerr << str << std::endl; });
	Logger Logger::out("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {msg}", [](const std::string &str) {std::cout << str << std::endl; });

	Logger::Logger() : m_Mute(false) {}
	Logger::Logger(const std::string &format) : m_Mute(false), m_Format(format){}
	Logger::Logger(const std::function<void(const std::string &)> &handler) : m_Mute(false)
	{
		m_Handlers.emplace_back(handler);
	}
	Logger::Logger(const std::string &format, const std::function<void(const std::string &)> &handler) : m_Mute(false), m_Format(format)
	{
		m_Handlers.emplace_back(handler);
	}

	void Logger::SetFormat(const std::string &format) { m_Format = format; }
	void Logger::Mute() { m_Mute = true; }
	void Logger::Unmute() { m_Mute = false; }

	static std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
	{
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return str;
	}

	void Logger::Log(const std::string &str)
	{
		std::string message = str;
		std::string log = m_Format;
		log = ReplaceAll(log, "{msg}", str);
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		size_t milliSeconds = (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm *localTime = std::localtime(&time);
		log = ReplaceAll(log, "{y}", std::to_string(localTime->tm_year + 1900));
		//month
		if (localTime->tm_mon >= 9)
			log = ReplaceAll(log, "{M}", std::to_string(localTime->tm_mon + 1));
		else
			log = ReplaceAll(log, "{M}", "0" + std::to_string(localTime->tm_mon + 1));
		//day
		if (localTime->tm_mday >= 10)
			log = ReplaceAll(log, "{d}", std::to_string(localTime->tm_mday));
		else
			log = ReplaceAll(log, "{d}", "0" + std::to_string(localTime->tm_mday));
		//hour
		if (localTime->tm_hour >= 10)
			log = ReplaceAll(log, "{h}", std::to_string(localTime->tm_hour));
		else
			log = ReplaceAll(log, "{h}", "0" + std::to_string(localTime->tm_hour));
		//minute
		if (localTime->tm_min >= 10)
			log = ReplaceAll(log, "{m}", std::to_string(localTime->tm_min));
		else
			log = ReplaceAll(log, "{m}", "0" + std::to_string(localTime->tm_min));
		//second
		if (localTime->tm_sec >= 10)
			log = ReplaceAll(log, "{s}", std::to_string(localTime->tm_sec));
		else
			log = ReplaceAll(log, "{s}", "0" + std::to_string(localTime->tm_sec));
		//millisecond
		if (milliSeconds >= 100)
			log = ReplaceAll(log, "{ms}", std::to_string(milliSeconds));
		else if (milliSeconds >= 10)
			log = ReplaceAll(log, "{ms}", "0" + std::to_string(milliSeconds));
		else
			log = ReplaceAll(log, "{ms}", "00" + std::to_string(milliSeconds));
		for (const auto &handler : m_Handlers)
			handler(log);
	}

	void Logger::Handle(const std::function<void(const std::string &)> &handler)
	{

	}
}