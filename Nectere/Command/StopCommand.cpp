#include "Command/StopCommand.hpp"

#include "Logger.hpp"

namespace Nectere
{
	namespace Command
	{
		StopCommand::StopCommand(Server *server): ANectereCommand(666, server) {}
		bool StopCommand::IsValid(const std::string &) const { return true; }
		void StopCommand::Treat(uint16_t sessionId, const std::string &)
		{
			Logger::out.Log('[', sessionId, "] Stopping server");
			m_Server->Stop();
		}
	}
}