#ifdef WIN32
#define WIN32_LEAN_AND_MEAN

#pragma once

#include <ctime>
#include <memory>
#include <vector>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Network/AServer.hpp"
#include "Network/WindowsNetworkUser.hpp"

namespace Nectere
{
	namespace Network
	{
		class WindowsNetworkServer : public AServer
		{
		private:
			bool m_IsStarted;
			fd_set m_FdRead;
			fd_set m_FdWrite;
			struct timeval m_Timeout;
			SOCKET m_ListenSocket = INVALID_SOCKET;
			bool m_WinSockStarted;
			std::vector<WindowsNetworkUser *> m_Sessions;

		private:
			void CloseSession(uint16_t);
			Concurrency::TaskResult AcceptConnection();

			template <class... t_Args>
			void DebugLog(const std::string &function, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->DebugLog("WindowsNetworkServer", function, std::forward<t_Args>(args)...);
			}

		public:
			WindowsNetworkServer(int, const std::string &, Concurrency::AThreadSystem *, const Logger *, UserManager *);
			bool IsStarted() { return m_IsStarted; }
			void Close();
			bool Start() override;
			void Stop() override { Close(); }
			~WindowsNetworkServer() { Close(); }
		};
	}
}

#endif