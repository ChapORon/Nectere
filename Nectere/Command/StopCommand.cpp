#include "Command/StopCommand.hpp"

#include "Concurrency/AThreadSystem.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "Network/AServer.hpp"

namespace Nectere
{
	namespace Command
	{
		StopCommand::StopCommand(const Ptr<Concurrency::AThreadSystem> &threadSystem) : ACommand(NECTERE_EVENT_STOP, "stop"), m_ThreadSystem(threadSystem) {}

		bool StopCommand::IsValid(const std::string &) const { return true; }

		void StopCommand::Treat(uint16_t sessionId, uint16_t, const std::string &)
		{
			if (IsAuthenticated(sessionId))
			{
				//LOG(LogType::Standard, '[', sessionId, "] Stopping server");
				SendEvent(sessionId, "Server Stopped");
				m_ThreadSystem->Stop();
			}
			else
				SendError(sessionId, "You don't have permission to stop server");
		}
	}
}