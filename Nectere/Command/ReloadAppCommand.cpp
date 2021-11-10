#include "Command/ReloadAppCommand.hpp"

#include "ApplicationManager.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		ReloadAppCommand::ReloadAppCommand(const Ptr<Network::AServer> &server, const Ptr<Concurrency::ThreadSystem> &threadSystem):
			ANectereCommand(NECTERE_EVENT_APP_RELOAD, "reload", server, threadSystem) {}

		bool ReloadAppCommand::IsValid(const std::string &) const { return true; }

		void ReloadAppCommand::Treat(uint16_t sessionId, const std::string &value)
		{
			for (Ptr<Application> application : m_ApplicationManager->GetApplications())
			{
				if (application->GetName() == value)
				{
					std::stringstream str;
					str << "Application \"" << value << "\" reload: ";
					uint16_t id = application->GetID();
					if (m_ApplicationManager->ReloadApplication(id)) //WARNING: Failing to reload app will result in its unloading
						str << "Success";
					else
						str << "Fail";
					SendEvent(sessionId, str.str());
					return;
				}
			}
			std::stringstream str;
			str << "Cannot reload \"" << value << "\": No such application";
			SendEvent(sessionId, str.str());
		}
	}
}