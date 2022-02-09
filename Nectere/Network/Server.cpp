#include "Network/Server.hpp"

namespace Nectere
{
	namespace Network
	{
		AServer *MakeServer(int port, const std::string &whitelistFile, Concurrency::AThreadSystem *threadSystem, const Logger *logger, UserManager *userManager)
		{
			#ifdef USE_BOOST
				return new BoostNetworkServer(port, whitelistFile, threadSystem, logger, userManager);
			#else
				#ifdef WIN32
					return new WindowsNetworkServer(port, whitelistFile, threadSystem, logger, userManager);
				#else
					return nullptr;
				#endif
			#endif
		}
	}
}