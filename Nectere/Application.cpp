#include "Application.hpp"

#include "Logger.hpp"

namespace Nectere
{
	Application::Application(uint16_t id, const std::string &name) : m_ID(id), m_Name(name), m_Handler(nullptr), m_UpdateElapsedTime(std::chrono::system_clock::now()) {}
	
	void Application::AddCommand(ACommand *command)
	{
		m_Commands.Add(command);
		if (m_Handler != nullptr)
			m_Handler->OnCommandAdded(command);
	}

	bool Application::IsEventAllowed(const Event &event)
	{
		if (ACommand *command = m_Commands.Get(event.m_EventCode))
			return command->IsValid(event.m_Data);
		return false;
	}

	void Application::Treat(uint16_t sessionID, const Event &event)
	{
		m_Commands.Get(event.m_EventCode)->Treat(sessionID, event.m_Data);
	}

	void Application::Update()
	{
		if (m_IsReloading.load())
			return;
		float deltaTime = std::chrono::duration<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - m_UpdateElapsedTime)).count();
		if (m_Handler != nullptr)
			m_Handler->OnUpdate(deltaTime);
		//Logger::out.Log(m_Name, ": Time since last update: ", deltaTime, 's');
		m_UpdateElapsedTime = std::chrono::system_clock::now();
	}

	void Application::BeforeReloading()
	{
		m_IsReloading.store(true);
		m_Commands.Clear();
		if (m_Handler != nullptr)
		{
			m_Handler->OnBeforeReload();
			m_Handler = nullptr;
		}
	}

	void Application::AfterReloading()
	{
		m_IsReloading.store(false);
		if (m_Handler != nullptr)
			m_Handler->OnAfterReload();
	}
}