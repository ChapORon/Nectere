#ifdef WIN32
#pragma once

#include <mutex>
#include <queue>
#include "Event.hpp"
#include "TaskResult.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>

namespace Nectere
{
	namespace Network
	{
		class IEventReceiver;
		class Session
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
			unsigned int m_ID;
			IEventReceiver *m_Handler;
			SOCKET m_Socket;
			std::atomic_bool m_Closed;
			std::mutex m_SendBufferMutex;
			std::queue<Nectere::Event> m_SendBuffer;

		private:
			Nectere::TaskResult ReadHeader(Header &);
			Nectere::TaskResult ReadMessage(const Header &);
			Nectere::TaskResult Write(const Nectere::Event &);

		public:
			Session(unsigned int, IEventReceiver *, const SOCKET &);
			SOCKET GetSocket() { return m_Socket; }
			void Send(const Nectere::Event &);
			void Clean();
			void Close();
			Nectere::TaskResult Read();
			Nectere::TaskResult Write();
			~Session() { Close(); }
		};
	}
}

#endif