#include "Command/CommandListCommand.hpp"

#include <sstream>
#include "ApplicationManager.hpp"
#include "Dp/Json.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		CommandListCommand::CommandListCommand(): ACommand(NECTERE_EVENT_CMD_LIST, "cmd-list") {}

		bool CommandListCommand::IsValid(const std::string &value) const { return !value.empty(); }

		void CommandListCommand::Treat(uint16_t sessionId, uint16_t, const std::string &value)
		{
			for (Ptr<Application> application : m_ApplicationManager->GetApplications())
			{
				if (application->GetName() == value)
				{
					std::vector<Dp::Node> commands;
					for (Ptr<ACommand> command : application->GetCommands())
					{
						Dp::Node commandNode;
						commandNode.Add("name", command->GetName());
						commandNode.Add("id", static_cast<int>(command->GetID()));
						commands.emplace_back(commandNode);
					}
					Dp::Node root;
					root.Add("commands", commands);
					SendEvent(sessionId, Dp::Json::Str(root, { 0, 0, false }));
					return;
				}
			}

			std::stringstream str;
			str << "Error with \"" << value << "\": No such application";
			SendEvent(sessionId, str.str());
		}
	}
}