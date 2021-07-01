#pragma once

#include <functional>
#include <mutex>
#include <sstream>
#include <vector>

namespace Nectere
{
	enum class LogType : int
	{
		Standard,
		Warning,
		Error,
		Verbose,
		Debug
	};

	class Logger
	{
	public:
		static Logger out;
		struct ScriptEngine
		{
			static Logger init;
			static Logger parsing;
			static Logger compilation;
			static Logger runtime;
		};

	private:
		bool m_Mute;
		std::string m_Format;
		mutable std::mutex m_Mutex;

	private:
		void LogFile(const std::string &, const std::string &, std::tm *) const;
		void Stringify(std::stringstream &os) const {}
		template <typename t_FirstElement>
		void Stringify(std::stringstream &os, const t_FirstElement &first) const
		{
			os << first;
		}
		template <typename t_FirstElement, typename... t_Args>
		void Stringify(std::stringstream &os, const t_FirstElement &first, const t_Args &... args) const
		{
			os << first;
			Stringify(os, args...);
		}

	public:
		Logger();
		Logger(const std::string &);
		void SetFormat(const std::string &);
		void Mute();
		void Unmute();

		template <class... t_Args>
		void Log(LogType logType, const t_Args &... args) const
		{
			std::stringstream stream;
			Stringify(stream, args...);
			Log(logType, stream.str());
		}

		template <class... t_Args>
		void DebugLog(const std::string &object, const std::string &function, const t_Args &... args) const
		{
			#ifdef DEBUG
				std::stringstream stream;
				stream << object << "::" << function << ": ";
				Stringify(stream, args...);
				Log(LogType::Debug, stream.str());
			#endif
		}

		void Log(LogType, const std::string &str) const;
	};
}

#define LOG Nectere::Logger::out.Log
#define DEBUGLOG Nectere::Logger::out.DebugLog
#define LOG_SCRIPT_INIT Nectere::Logger::ScriptEngine::init.Log
#define LOG_SCRIPT_PARSING Nectere::Logger::ScriptEngine::parsing.Log
#define LOG_SCRIPT_COMPILATION Nectere::Logger::ScriptEngine::compilation.Log
#define LOG_SCRIPT_RUNTIME Nectere::Logger::ScriptEngine::runtime.Log