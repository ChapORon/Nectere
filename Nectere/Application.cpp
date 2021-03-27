#include "Application.hpp"

namespace Nectere
{
	Application::Application(uint16_t id, const std::string &name) : m_ID(id), m_Name(name) {}
	
	void Application::AddCommand(const std::shared_ptr<ACommand> &command)
	{
		m_Commands.Add(command);
	}

	bool Application::IsEventAllowed(const Event &event)
	{
		if (const std::shared_ptr<ACommand> &command = m_Commands.Get(event.m_EventCode))
			return command->IsValid(event.m_Data);
		return false;
	}

	void Application::Treat(uint16_t sessionID, const Event &event)
	{
		m_Commands.Get(event.m_EventCode)->Treat(sessionID, event.m_Data);
	}
}