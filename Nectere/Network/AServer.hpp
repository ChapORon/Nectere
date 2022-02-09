#pragma once

#include <unordered_set>
#include "Event.hpp"
#include "Logger.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	class Logger;
	class UserManager;
    namespace Concurrency
    {
        class AThreadSystem;
    }

	namespace Network
	{
		class AServer
		{
		protected:
			bool m_WhitelistEnable{ false };
			bool m_IsStarted{ false };
			int m_Port;
            Concurrency::AThreadSystem *m_ThreadSystem;
			const Logger *m_Logger = nullptr;
			UserManager *m_UserManager;
			std::unordered_set<std::string> m_WhitelistedIP;

		protected:
			AServer(int, const std::string &, Concurrency::AThreadSystem *, const Logger *, UserManager *);

			bool IsIPWhitelisted(const std::string &);

			template <class... t_Args>
			void Log(LogType logType, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->Log(logType, std::forward<t_Args>(args)...);
			}

		public:
			virtual bool Start() = 0;
			virtual void Stop() = 0;
		};
	}
}