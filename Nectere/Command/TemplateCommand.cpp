#include "Command/StopCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		TemplateCommand::TemplateCommand(Network::AServer *server, Concurrency::ThreadSystem *threadSystem):
			ANectereCommand(NECTERE_EVENT_TEMPLATE, "stop", server, threadSystem) {}

		bool TemplateCommand::IsValid(const std::string &) const { return true; }

		void TemplateCommand::Treat(uint16_t sessionId, const std::string &)
		{
			LOG(LogType::Standard, '[', sessionId, "] Stopping server");
			SendEvent(sessionId, "Command Received");
		}
	}
}