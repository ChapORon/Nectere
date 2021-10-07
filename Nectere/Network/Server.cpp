#include "Network/Server.hpp"

namespace Nectere
{
	namespace Network
	{
		AServer *MakeServer(int port, Concurrency::ThreadSystem *threadSystem, IEventReceiver *handler)
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