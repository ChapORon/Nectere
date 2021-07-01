#pragma once

#include "ACommand.hpp"

namespace Nectere
{
	class ThreadSystem;
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
			ThreadSystem *m_ThreadSystem;

		public:
			ANectereCommand(uint16_t, Network::AServer *, ThreadSystem *);
		};
	}
}