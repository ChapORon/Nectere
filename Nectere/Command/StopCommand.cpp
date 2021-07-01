#include "Command/StopCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "Network/AServer.hpp"
#include "ThreadSystem.hpp"

namespace Nectere
{
	namespace Command
	{
		StopCommand::StopCommand(Network::AServer *server, ThreadSystem *threadSystem): ANectereCommand(NECTERE_EVENT_STOP, server, threadSystem) {}
		bool StopCommand::IsValid(const std::string &) const { return true; }
		void StopCommand::Treat(uint16_t sessionId, const std::string &)
		{
			LOG(LogType::Standard, '[', sessionId, "] Stopping server");
			m_Server->Stop();
			m_ThreadSystem->Stop();
		}
	}
}