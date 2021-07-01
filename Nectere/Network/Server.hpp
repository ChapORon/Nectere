#pragma once

#ifdef USE_BOOST
	#include "Network/Boost_Server.hpp"
#else
	#ifdef WIN32
		#include "Network/Windows_Server.hpp"
	#endif
#endif

namespace Nectere
{
	class ThreadSystem;
	namespace Network
	{
		class AServer;
		class IEventReceiver;

		AServer *MakeServer(int port, ThreadSystem * threadSystem, IEventReceiver *handler)
		{
			#ifdef USE_BOOST
				return new Boost_Server(port, threadSystem, handler);
			#else
				#ifdef WIN32
					return new Windows_Server(port, threadSystem, handler);
				#else
					return nullptr;
				#endif
			#endif
		}
	}
}