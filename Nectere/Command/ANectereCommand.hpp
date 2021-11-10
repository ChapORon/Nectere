#pragma once

#include "ACommand.hpp"
#include "Ptr.hpp"

namespace Nectere
{
    namespace Concurrency
    {
        class ThreadSystem;
    }

	namespace Network
	{
		class AServer;
	}

	namespace Command
	{
		class ANectereCommand: public ACommand
		{
		protected:
			Ptr<Network::AServer> m_Server;
			Ptr<Concurrency::ThreadSystem> m_ThreadSystem;

		public:
			ANectereCommand(uint16_t, const std::string &, const Ptr<Network::AServer> &, const Ptr<Concurrency::ThreadSystem> &);
		};
	}
}