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
	class AThreadSystem;
	class UserManager;
	namespace Network
	{
		class AServer;
		AServer *MakeServer(int, const std::string &, Concurrency::AThreadSystem *, const Logger *, UserManager *);
	}
}