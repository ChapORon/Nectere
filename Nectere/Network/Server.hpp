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
	class UserManager;
	namespace Network
	{
		class AServer;

		AServer *MakeServer(int port, Concurrency::ThreadSystem *threadSystem, UserManager *userManager)
		{
			#ifdef USE_BOOST
				return new Boost_Server(port, threadSystem, userManager);
			#else
				#ifdef WIN32
					return new Windows_Server(port, threadSystem, userManager);
				#else
					return nullptr;
				#endif
			#endif
		}
	}
}