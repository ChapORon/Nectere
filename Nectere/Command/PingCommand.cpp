#include "Command/PingCommand.hpp"

#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		PingCommand::PingCommand(): ACommand(NECTERE_EVENT_PING, "ping") {}

		bool PingCommand::IsValid(const std::string &) const { return true; }

		void PingCommand::Treat(uint16_t sessionId, uint16_t, const std::string &data)
		{
			SendEvent(sessionId, data);
		}
	}
}