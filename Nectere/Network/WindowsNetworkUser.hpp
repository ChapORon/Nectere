#ifdef WIN32
#pragma once

#include <mutex>
#include <queue>
#include "AUser.hpp"
#include "Event.hpp"
#include "Concurrency/TaskResult.hpp"

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
			struct Header
			{
				uint16_t applicationID;
				uint16_t messageType;
				uint16_t apiVersion;
				uint64_t messageLength;
			};

		private:
			SOCKET m_Socket;
			std::atomic_bool m_Closed;
			std::mutex m_SendBufferMutex;
			std::queue<Nectere::Event> m_SendBuffer;

		private:
            Concurrency::TaskResult ReadHeader(Header &);
            Concurrency::TaskResult ReadMessage(const Header &);
            Concurrency::TaskResult Write(const Nectere::Event &);

		public:
			WindowsNetworkUser(const SOCKET &);
			SOCKET GetSocket() { return m_Socket; }
			void Send(const Nectere::Event &);
			void Clean();
			void Close();
            Concurrency::TaskResult Read();
            Concurrency::TaskResult Write();
			~WindowsNetworkUser() { Close(); }
		};
	}
}

#endif