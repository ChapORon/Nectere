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
#include "Network/Windows_Session.hpp"

namespace Nectere
{
	namespace Network
	{
		class Windows_Server : public AServer
		{
		private:
			bool m_IsStarted;
			fd_set m_FdRead;
			fd_set m_FdWrite;
			struct timeval m_Timeout;
			SOCKET m_ListenSocket = INVALID_SOCKET;
			bool m_WinSockStarted;
			std::vector<Windows_Session *> m_Sessions;

		private:
			Concurrency::TaskResult AcceptConnection();

		public:
			Windows_Server(int, Concurrency::ThreadSystem *, UserManager *);
			bool IsStarted() { return m_IsStarted; }
			void Close();
			bool Start() override;
			void Stop() override { Close(); }
			~Windows_Server() { Close(); }
		};
	}
}

#endif