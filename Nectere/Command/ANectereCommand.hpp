#pragma once

#include "ACommand.hpp"

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
			Network::AServer *m_Server;
            Concurrency::ThreadSystem *m_ThreadSystem;

		public:
			ANectereCommand(uint16_t, const std::string &, Network::AServer *, Concurrency::ThreadSystem *);
		};
	}
}