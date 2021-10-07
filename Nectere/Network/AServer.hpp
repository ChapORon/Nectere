#pragma once

#include "Event.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	class UserManager;
    namespace Concurrency
    {
        class ThreadSystem;
    }

	namespace Network
	{
		class AServer
		{
		protected:
			bool m_IsStarted{ false };
			int m_Port;
            Concurrency::ThreadSystem *m_ThreadSystem;
			UserManager *m_UserManager;

		protected:
			AServer(int port, Concurrency::ThreadSystem *threadSystem, UserManager *userManager): m_Port(port), m_ThreadSystem(threadSystem), m_UserManager(userManager) {}

		public:
			virtual bool Start() = 0;
			virtual void Stop() = 0;
		};
	}
}