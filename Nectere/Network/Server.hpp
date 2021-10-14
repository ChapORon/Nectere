#pragma once

#ifdef USE_BOOST
	#include "Network/BoostNetworkServer.hpp"
#else
	#ifdef WIN32
		#include "Network/WindowsNetworkServer.hpp"
	#endif
#endif

namespace Nectere
{
	class ThreadSystem;
	class UserManager;
	namespace Network
	{
		class AServer;

		AServer *MakeServer(int port, Concurrency::ThreadSystem *threadSystem, UserManager *userManager)
		{
			#ifdef USE_BOOST
				return new BoostNetworkServer(port, threadSystem, userManager);
			#else
				#ifdef WIN32
					return new WindowsNetworkServer(port, threadSystem, userManager);
				#else
					return nullptr;
				#endif
			#endif
		}
	}
}