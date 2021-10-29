#include "Command/StopCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "Network/AServer.hpp"
#include "Concurrency/ThreadSystem.hpp"

namespace Nectere
{
	namespace Command
	{
		StopCommand::StopCommand(Network::AServer *server, Concurrency::ThreadSystem *threadSystem): ANectereCommand(NECTERE_EVENT_STOP, "stop", server, threadSystem) {}
		bool StopCommand::IsValid(const std::string &) const { return true; }
		void StopCommand::Treat(uint16_t sessionId, const std::string &)
		{
			LOG(LogType::Standard, '[', sessionId, "] Stopping server");
			SendEvent(sessionId, "Server Stopped");
			m_Server->Stop();
			m_ThreadSystem->Stop();
		}
	}
}