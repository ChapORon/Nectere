#ifdef WIN32
#include "Network/Windows_Server.hpp"

#include "Logger.hpp"
#include "ThreadSystem.hpp"

namespace Nectere
{
	namespace Network
	{
		Windows_Server::Windows_Server(int port, ThreadSystem *threadSystem, IEventReceiver *handler) : AServer(port, threadSystem, handler)
		{
			m_Timeout.tv_sec = 60;
			m_Timeout.tv_usec = 0;
			m_IsStarted = false;
			m_WinSockStarted = false;
		}

		bool Windows_Server::Start()
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
									LOG(LogType::Error, "Cannot change blocking mode of server socket: ", WSAGetLastError());
								else
								{
									m_IsStarted = true;
									m_ThreadSystem->AddTask(this, &Windows_Server::AcceptConnection);
									return true;
								}
							}
							else
								LOG(LogType::Error, "Cannot listen to server socket: ", WSAGetLastError());
						}
						else
							LOG(LogType::Error, "Cannot bind server socket: ", WSAGetLastError());
					}
					else
						LOG(LogType::Error, "Cannot create socket for server on port ", m_Port, ": ", WSAGetLastError());
				}
				else
					LOG(LogType::Error, "Cannot start Windows Socket");
				m_ListenSocket = INVALID_SOCKET;
			}
			return false;
		}

		Nectere::TaskResult Windows_Server::AcceptConnection()
		{
			FD_ZERO(&m_FdRead);
			FD_ZERO(&m_FdWrite);
			FD_SET(0, &m_FdRead);
			FD_SET(m_ListenSocket, &m_FdRead);
			for (Windows_Session *session : m_Sessions)
			{
				int sessionSocket = session->GetSocket();
				FD_SET(sessionSocket, &m_FdRead);
				FD_SET(sessionSocket, &m_FdWrite);
			}
			int ret = select(0, &m_FdRead, &m_FdWrite, nullptr, &m_Timeout);
			if (ret < 0)
			{
				LOG(LogType::Error, "Error on select: ", WSAGetLastError());
				return TaskResult::Fail;
			}
			else if (ret == 0)
			{
				LOG(LogType::Warning, "No network activity in the last ", m_Timeout.tv_sec, '.', m_Timeout.tv_usec, " seconds");
				return TaskResult::NeedUpdate;
			}
			if (FD_ISSET(m_ListenSocket, &m_FdRead))
			{
				SOCKET ClientSocket = accept(m_ListenSocket, nullptr, nullptr);
				if (ClientSocket != INVALID_SOCKET)
				{
					LOG(LogType::Verbose, "New connection");
				}
				else
				{
					LOG(LogType::Error, "Error while accepting new connection: ", WSAGetLastError());
					Close();
					return TaskResult::Fail;
				}
			}
			for (const auto &session : m_Sessions)
			{
				if (FD_ISSET(session->GetSocket(), &m_FdRead))
					m_ThreadSystem->AddTask(session, &Windows_Session::Read);
				if (FD_ISSET(session->GetSocket(), &m_FdWrite))
					m_ThreadSystem->AddTask(session, &Windows_Session::Write);
			}
			return TaskResult::NeedUpdate;
		}

		void Windows_Server::Close()
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

			for (Windows_Session *session : m_Sessions)
				delete(session);
			m_Sessions.clear();
		}
	}
}
#endif