#include "Command/ReloadAppCommand.hpp"

#include "ApplicationManager.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		ReloadAppCommand::ReloadAppCommand(): ACommand(NECTERE_EVENT_APP_RELOAD, "reload") {}

		bool ReloadAppCommand::IsValid(const std::string &) const { return true; }

		void ReloadAppCommand::Treat(uint16_t sessionId, uint16_t, const std::string &value)
		{
			if (value == "Nectere")
			{
				SendError(sessionId, "Cannot reload base application Nectere");
				return;
			}
			for (Ptr<Application> application : m_ApplicationManager->GetApplications())
			{
				if (application->GetName() == value)
				{
					std::stringstream str;
					str << "Application \"" << value << "\" reload: ";
					uint16_t id = application->GetID();
					if (m_ApplicationManager->ReloadApplication(id)) //WARNING: Failing to reload app will result in its unloading
					{
						str << "Success";
						SendEvent(sessionId, str.str());
					}
					else
					{
						str << "Fail";
						SendError(sessionId, str.str());
					}
					return;
				}
			}
			std::stringstream str;
			str << "Cannot reload \"" << value << "\": No such application";
			SendError(sessionId, str.str());
		}
	}
}