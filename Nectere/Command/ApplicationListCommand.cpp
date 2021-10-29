#include "Command/ApplicationListCommand.hpp"

#include "ApplicationManager.hpp"
#include "Dp/Json.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		ApplicationListCommand::ApplicationListCommand(Network::AServer *server, Concurrency::ThreadSystem *threadSystem):
			ANectereCommand(NECTERE_EVENT_APP_LIST, "app-list", server, threadSystem) {}

		bool ApplicationListCommand::IsValid(const std::string &) const { return true; }

		void ApplicationListCommand::Treat(uint16_t sessionId, const std::string &)
		{
			std::vector<Dp::Node> applications;
			for (Ptr<Application> application : m_ApplicationManager->GetApplications())
			{
				Dp::Node commandNode;
				commandNode.Add("name", application->GetName());
				commandNode.Add("id", static_cast<int>(application->GetID()));
				applications.emplace_back(commandNode);
			}
			Dp::Node root;
			root.Add("applications", applications);
			SendEvent(sessionId, Dp::Json::Str(root, { 0, 0, false }));
		}
	}
}