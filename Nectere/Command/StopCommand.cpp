#include "Command/StopCommand.hpp"

#include <iostream>

namespace Nectere
{
	namespace Command
	{
		StopCommand::StopCommand(Server *server): ANectereCommand(666, server) {}
		bool StopCommand::IsValid(const std::string &) const { return true; }
		void StopCommand::Treat(uint16_t sessionId, const std::string &)
		{
			std::cout << '[' << sessionId << "] Stopping server" << std::endl;
			m_Server->Stop();
		}
	}
}