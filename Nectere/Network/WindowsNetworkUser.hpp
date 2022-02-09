#ifdef WIN32
#pragma once

#include <mutex>
#include <queue>
#include "AUser.hpp"
#include "Concurrency/TaskResult.hpp"
#include "Event.hpp"
#include "Logger.hpp"
#include "Network/Header.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>

namespace Nectere
{
	namespace Network
	{
		class WindowsNetworkUser: public AUser
		{
		private:
			const Logger *m_Logger = nullptr;
			SOCKET m_Socket;
			std::atomic_bool m_Closed;
			std::mutex m_SendBufferMutex;
			std::queue<Nectere::Event> m_SendBuffer;

		private:
            Concurrency::TaskResult ReadHeader(Header &);
            Concurrency::TaskResult ReadMessage(const Header &);
            Concurrency::TaskResult Write(const Nectere::Event &);
			void SendError(uint16_t, const std::string &);
			void Clean();

			template <class... t_Args>
			void Log(LogType logType, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->Log(logType, std::forward<t_Args>(args)...);
			}

			template <class... t_Args>
			void DebugLog(const std::string &function, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->DebugLog("WindowsNetworkUser", function, std::forward<t_Args>(args)...);
			}

		public:
			WindowsNetworkUser(const Logger *, const SOCKET &);
			SOCKET GetSocket() { return m_Socket; }
			void Send(const Nectere::Event &);
			void OnClose() override;
            Concurrency::TaskResult Read();
            Concurrency::TaskResult Write();
			~WindowsNetworkUser() { Clean(); }
		};
	}
}

#endif