#pragma once

#include "ACommand.hpp"
#include "Server.hpp"

namespace Nectere
{
	namespace Command
	{
		class ANectereCommand: public ACommand
		{
		protected:
			Server *m_Server;

		public:
			ANectereCommand(uint16_t, Server *);
		};
	}
}