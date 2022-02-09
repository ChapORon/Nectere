#include "Command/UnloadAppCommand.hpp"

#include "ApplicationManager.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		UnloadAppCommand::UnloadAppCommand(): ACommand(NECTERE_EVENT_APP_UNLOAD, "unload") {}

		bool UnloadAppCommand::IsValid(const std::string &) const { return true; }

		void UnloadAppCommand::Treat(uint16_t sessionId, uint16_t, const std::string &value)
		{
			if (value == "Nectere")
			{
				SendError(sessionId, "Cannot unload base application Nectere");
				return;
			}
			for (Ptr<Application> application : m_ApplicationManager->GetApplications())
			{
				if (application->GetName() == value)
				{
					m_ApplicationManager->UnloadApplication(application->GetID());
					std::stringstream str;
					str << "Application \"" << value << "\" unloaded";
					SendEvent(sessionId, str.str());
					return;
				}
			}
			std::stringstream str;
			str << "Cannot unload \"" << value << "\": No such application";
			SendError(sessionId, str.str());
		}
	}
}