#ifdef WIN32
#include "Network/WindowsNetworkServer.hpp"

#include "Logger.hpp"
#include "Concurrency/ThreadSystem.hpp"
#include "UserManager.hpp"

namespace Nectere
{
	namespace Network
	{
		WindowsNetworkServer::WindowsNetworkServer(int port, const std::string &whitelistFile, Concurrency::AThreadSystem *threadSystem, const Logger *logger, UserManager *userManager) :
			AServer(port, whitelistFile, threadSystem, logger, userManager)
		{
			m_Timeout.tv_sec = 60;
			m_Timeout.tv_usec = 0;
			m_IsStarted = false;
			m_WinSockStarted = false;
		}

		bool WindowsNetworkServer::Start()
		{
			if (!m_IsStarted)
			{
				WSADATA wsaData;
				SOCKADDR_IN addr;

				// Initialize Winsock
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
				{
					m_WinSockStarted = true;
					addr.sin_family = AF_INET;
					addr.sin_addr.s_addr = htonl(INADDR_ANY);
					addr.sin_port = htons(m_Port);

					// Create a SOCKET for connecting to server
					m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
					if (m_ListenSocket != INVALID_SOCKET)
					{
						// Setup the TCP listening socket
						if (bind(m_ListenSocket, (PSOCKADDR)&addr, sizeof(addr)) != -1)
						{
							if (listen(m_ListenSocket, SOMAXCONN) != -1)
							{
								ULONG mode = 1;
								if (ioctlsocket(m_ListenSocket, FIONBIO, &mode) == SOCKET_ERROR)
									Log(LogType::Error, "Cannot change blocking mode of server socket: ", WSAGetLastError());
								else
								{
									m_IsStarted = true;
									m_ThreadSystem->AddTask(this, &WindowsNetworkServer::AcceptConnection);
									return true;
								}
							}
							else
								Log(LogType::Error, "Cannot listen to server socket: ", WSAGetLastError());
						}
						else
							Log(LogType::Error, "Cannot bind server socket: ", WSAGetLastError());
					}
					else
						Log(LogType::Error, "Cannot create socket for server on port ", m_Port, ": ", WSAGetLastError());
				}
				else
					Log(LogType::Error, "Cannot start Windows Socket");
				m_ListenSocket = INVALID_SOCKET;
			}
			return false;
		}

		void WindowsNetworkServer::CloseSession(uint16_t id)
		{
			Log(LogType::Standard, "Closing session with ID: ", id);
			m_UserManager->RemoveUser(id);
			m_Sessions.erase(std::find_if(m_Sessions.begin(), m_Sessions.end(), [=](const WindowsNetworkUser *session) { return session->GetID() == id; }));
		}

		Nectere::Concurrency::TaskResult WindowsNetworkServer::AcceptConnection()
		{
			FD_ZERO(&m_FdRead);
			FD_ZERO(&m_FdWrite);
			FD_SET(0, &m_FdRead);
			FD_SET(m_ListenSocket, &m_FdRead);
			for (WindowsNetworkUser *session : m_Sessions)
			{
				int sessionSocket = session->GetSocket();
				FD_SET(sessionSocket, &m_FdRead);
				FD_SET(sessionSocket, &m_FdWrite);
			}
			int ret = select(0, &m_FdRead, &m_FdWrite, nullptr, &m_Timeout);
			if (ret < 0)
			{
				Log(LogType::Error, "Error on select: ", WSAGetLastError());
				return Concurrency::TaskResult::Fail;
			}
			else if (ret == 0)
			{
				Log(LogType::Warning, "No network activity in the last ", m_Timeout.tv_sec, '.', m_Timeout.tv_usec, " seconds");
				return Concurrency::TaskResult::NeedUpdate;
			}
			if (FD_ISSET(m_ListenSocket, &m_FdRead))
			{
				sockaddr addr;
				SOCKET ClientSocket = accept(m_ListenSocket, &addr, nullptr);
				if (ClientSocket != INVALID_SOCKET)
				{
					std::string clientIP = addr.sa_data;
					Log(LogType::Standard, "New connection on IP ", clientIP);
					if (IsIPWhitelisted(clientIP))
					{
// 						if (auto networkUser = m_UserManager->AddUser<WindowsNetworkUser>(m_Logger, ClientSocket))
// 						{
// 							uint16_t id = networkUser->GetID();
// 							Log(LogType::Standard, "New session opened with ID: ", id);
// 							m_Sessions.emplace_back(networkUser);
// 						}
					}
				}
				else
				{
					Log(LogType::Error, "Error while accepting new connection: ", WSAGetLastError());
					Close();
					return Concurrency::TaskResult::Fail;
				}
			}
			for (const auto &session : m_Sessions)
			{
				if (FD_ISSET(session->GetSocket(), &m_FdRead))
					m_ThreadSystem->AddTask(session, &WindowsNetworkUser::Read);
				if (FD_ISSET(session->GetSocket(), &m_FdWrite))
					m_ThreadSystem->AddTask(session, &WindowsNetworkUser::Write);
			}
			return Concurrency::TaskResult::NeedUpdate;
		}

		void WindowsNetworkServer::Close()
		{
			if (m_ListenSocket != INVALID_SOCKET)
			{
				closesocket(m_ListenSocket);
			}
			if (m_WinSockStarted)
			{
				WSACleanup();
				m_WinSockStarted = false;
			}

			for (WindowsNetworkUser *session : m_Sessions)
				m_UserManager->RemoveUserInstant(session->GetID());
			m_Sessions.clear();
		}
	}
}
#endif