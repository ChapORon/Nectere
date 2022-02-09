#pragma once

#include <functional>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "nectere_export.h"
#include "Event.hpp"

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

	template <typename t_Element>
	struct Stringifier
	{
		static void Stringify(std::stringstream &os, const t_Element &first)
		{
			os << first;
		}
	};

	template <typename T, typename U>
	struct Stringifier<std::unordered_map<T, U>>
	{
		static void Stringify(std::stringstream &os, const std::unordered_map<T, U> &first)
		{
			bool filled = false;
			os << "[";
			for (const auto &pair : first)
			{
				if (filled)
					os << ", ";
				os << "[";
				Stringifier<T>::Stringify(os, pair.first);
				os << ", ";
				Stringifier<U>::Stringify(os, pair.second);
				os << "]";
			}
			os << "]";
		}
	};

	template <typename T>
	struct Stringifier<std::unordered_set<T>>
	{
		static void Stringify(std::stringstream &os, const std::unordered_set<T> &first)
		{
			bool filled = false;
			os << "[";
			for (const T &element : first)
			{
				if (filled)
					os << ", ";
				Stringifier<T>::Stringify(os, element);
			}
			os << "]";
		}
	};

	template <typename T>
	struct Stringifier<std::vector<T>>
	{
		static void Stringify(std::stringstream &os, const std::vector<T> &first)
		{
			bool filled = false;
			os << "[";
			for (const T &element : first)
			{
				if (filled)
					os << ", ";
				Stringifier<T>::Stringify(os, element);
			}
			os << "]";
		}
	};

	template<>
	struct Stringifier<Version>
	{
		static void Stringify(std::stringstream &os, const Version &version)
		{
			os << version.m_Major << '.' << version.m_Minor << '.' << version.m_Patch;
		}
	};

	template<>
	struct Stringifier<Event>
	{
		static void Stringify(std::stringstream &os, const Event &event)
		{
			os << "[application:" << event.m_ApplicationID << ", event:" << event.m_EventCode << ", api:";
			Stringifier<Version>::Stringify(os, event.m_APIVersion);
			os << "] " << event.m_Data;
		}
	};

	class Logger
	{
		friend class Manager;
	private:
		bool m_IsVerbose = false;
		bool m_LogInFile = false;
		std::string m_LogFilePath;
		std::string m_Format;
		mutable std::mutex m_Mutex;

	private:
		void Init(bool, bool, const std::string &);
		void LogFile(const std::string &, const std::string &, std::tm *) const;
		void Stringify(std::stringstream &os) const {}
		template <typename t_FirstElement>
		void Stringify(std::stringstream &os, const t_FirstElement &first) const
		{
			Stringifier<t_FirstElement>::Stringify(os, first);
		}
		template <typename t_FirstElement, typename... t_Args>
		void Stringify(std::stringstream &os, const t_FirstElement &first, t_Args &&... args) const
		{
			Stringifier<t_FirstElement>::Stringify(os, first);
			Stringify(os, std::forward<t_Args>(args)...);
		}

	public:
		NECTERE_EXPORT Logger();
		NECTERE_EXPORT Logger(const std::string &);

		template <class... t_Args>
		void Log(LogType logType, t_Args &&... args) const
		{
			std::stringstream stream;
			Stringify(stream, std::forward<t_Args>(args)...);
			LogStr(logType, stream.str());
		}

		template <class... t_Args>
		void DebugLog(const std::string &object, const std::string &function, t_Args &&... args) const
		{
			#ifdef DEBUG
				Log(LogType::Debug, object, "::", function, ": ", std::forward<t_Args>(args)...);
			#endif
		}

		NECTERE_EXPORT void LogStr(LogType, const std::string &str) const;
	};
}