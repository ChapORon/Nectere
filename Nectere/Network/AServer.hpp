#pragma once

#include "Event.hpp"
#include "IDGenerator.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	class ThreadSystem;
	namespace Network
	{
		class AServer
		{
		protected:
			bool m_IsStarted{ false };
			int m_Port;
			ThreadSystem *m_ThreadSystem;
			IEventReceiver *m_EventReceiver;
			IDGenerator m_SessionIDGenerator;

		protected:
			AServer(int port, ThreadSystem *threadSystem, IEventReceiver *handler): m_Port(port), m_ThreadSystem(threadSystem), m_EventReceiver(handler) {}

		public:
			virtual bool Start() = 0;
			virtual void Stop() = 0;
			virtual void Send(uint16_t, const Event &) = 0;
		};
	}
}