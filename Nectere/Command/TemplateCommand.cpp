#include "Command/TemplateCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		TemplateCommand::TemplateCommand(): ACommand(NECTERE_EVENT_TEMPLATE, "template") {}

		bool TemplateCommand::IsValid(const std::string &) const { return true; }

		void TemplateCommand::Treat(uint16_t sessionId, uint16_t, const std::string &)
		{
			SendError(sessionId, "Command Not Implemented Yet");
		}
	}
}