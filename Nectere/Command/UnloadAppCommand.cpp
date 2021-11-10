#include "Command/UnloadAppCommand.hpp"

#include "ApplicationManager.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		UnloadAppCommand::UnloadAppCommand(const Ptr<Network::AServer> &server, const Ptr<Concurrency::ThreadSystem> &threadSystem):
			ANectereCommand(NECTERE_EVENT_APP_UNLOAD, "unload", server, threadSystem) {}

		bool UnloadAppCommand::IsValid(const std::string &) const { return true; }

		void UnloadAppCommand::Treat(uint16_t sessionId, const std::string &value)
		{
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
			SendEvent(sessionId, str.str());
		}
	}
}