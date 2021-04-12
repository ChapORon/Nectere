#pragma once

#include <functional>
#include <sstream>
#include <vector>

namespace Nectere
{
	class Logger
	{
	public:
		static Logger err;
		static Logger out;

	private:
		bool m_Mute;
		std::string m_Format;
		std::vector<std::function<void(const std::string &)>> m_Handlers;

	private:
		void Stringify(std::stringstream &os) {}
		template <class t_FirstElement>
		void Stringify(std::stringstream &os, const t_FirstElement &first)
		{
			os << first;
		}
		template <class t_FirstElement, class... t_Args>
		void Stringify(std::stringstream &os, const t_FirstElement &first, const t_Args &... args)
		{
			os << first;
			Stringify(os, args...);
		}

	public:
		Logger();
		Logger(const std::string &);
		Logger(const std::function<void(const std::string &)> &);
		Logger(const std::string &, const std::function<void(const std::string &)> &);
		void SetFormat(const std::string &);
		void Mute();
		void Unmute();

		template <class... t_Args>
		void Log(const t_Args &... args)
		{
			std::stringstream stream;
			Stringify(stream, args...);
			Log(stream.str());
		}

		template <class... t_Args>
		void DebugLog(const std::string &object, const std::string &function, const t_Args &... args)
		{
			std::stringstream stream;
			stream << object << "::" << function << ": ";
			Stringify(stream, args...);
			Log(stream.str());
		}

		void Log(const std::string &str);
		void DebugLog(const std::string &object, const std::string &function, const std::string &str) { Log(object + "::" + function + ": " + str); }
		void Handle(const std::function<void(const std::string &)> &handler);
	};
}