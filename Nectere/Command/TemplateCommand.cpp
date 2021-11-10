#include "Command/TemplateCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		TemplateCommand::TemplateCommand(const Ptr<Network::AServer> &server, const Ptr<Concurrency::ThreadSystem> &threadSystem):
			ANectereCommand(NECTERE_EVENT_TEMPLATE, "stop", server, threadSystem) {}

		bool TemplateCommand::IsValid(const std::string &) const { return true; }

		void TemplateCommand::Treat(uint16_t sessionId, const std::string &)
		{
			SendEvent(sessionId, "Command Not Implemented Yet");
		}
	}
}