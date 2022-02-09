#include "Application.hpp"

#include "ApplicationManager.hpp"
#include "Logger.hpp"
#include "VersionHelper.hpp"

namespace Nectere
{
	Application::Application(uint16_t id, const std::string &name, const Ptr<ApplicationManager> &applicationManager) :
		m_ID(id),
		m_Name(name),
		m_ApplicationManager(applicationManager),
		m_Handler(nullptr),
		m_UpdateElapsedTime(std::chrono::system_clock::now()) {}
	
	ACommand *Application::InternalAddCommand(ACommand *command)
	{
		if (command)
		{
			command->SetApplication(Ptr(this), m_ApplicationManager);
			command->OnInit();
			m_Commands.Add(command);
			if (m_Handler != nullptr)
				m_Handler->OnCommandAdded(command);
		}
		return command;
	}

	void Application::SendEvent(uint16_t id, bool error, uint16_t commandID, const std::string &data)
	{
		Event event;
		event.m_Error = error;
		event.m_ApplicationID = m_ID;
		event.m_EventCode = commandID;
		event.m_Data = data;
		VersionHelper::SetupEvent(event);
		m_ApplicationManager->SendEvent(id, event);
	}

	void Application::SendEvent(const std::vector<uint16_t> &ids, bool error, uint16_t commandID, const std::string &data)
	{
		Event event;
		event.m_Error = error;
		event.m_ApplicationID = m_ID;
		event.m_EventCode = commandID;
		event.m_Data = data;
		VersionHelper::SetupEvent(event);
		m_ApplicationManager->SendEvent(ids, event);
	}

	void Application::SendEvent(uint16_t id, uint16_t commandID, const std::string &data)
	{
		SendEvent(id, false, commandID, data);
	}

	void Application::SendEvent(const std::vector<uint16_t> &ids, uint16_t commandID, const std::string &data)
	{
		SendEvent(ids, false, commandID, data);
	}

	void Application::SendError(uint16_t id, uint16_t commandID, const std::string &data)
	{
		SendEvent(id, true, commandID, data);
	}

	void Application::SendError(const std::vector<uint16_t> &ids, uint16_t commandID, const std::string &data)
	{
		SendEvent(ids, true, commandID, data);
	}

	bool Application::IsEventAllowed(const Event &event, bool &eventExist)
	{
		eventExist = false;
		if (Ptr<ACommand> &command = m_Commands.Get(event.m_EventCode))
		{
			eventExist = true;
			return command->IsValid(event.m_Data);
		}
		return false;
	}

	void Application::Treat(uint16_t sessionID, const Event &event)
	{
		m_Commands.Get(event.m_EventCode)->Treat(sessionID, event.m_CanalID, event.m_Data);
	}

	void Application::Update()
	{
		if (m_IsReloading.load())
			return;
		for (auto command : m_Commands)
			command->Update();
		float deltaTime = std::chrono::duration<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - m_UpdateElapsedTime)).count();
		if (m_Handler != nullptr)
			m_Handler->OnUpdate(deltaTime);
		//Logger::out.Log(m_Name, ": Time since last update: ", deltaTime, 's');
		m_UpdateElapsedTime = std::chrono::system_clock::now();
	}

	void Application::BeforeReloading(Dp::Node &root)
	{
		m_IsReloading.store(true);
		m_Commands.Clear();
		if (m_Handler != nullptr)
		{
			m_Handler->OnBeforeReload(root);
			m_Handler = nullptr;
		}
	}

	void Application::AfterReloading(const Dp::Node &root)
	{
		m_IsReloading.store(false);
		if (m_Handler != nullptr)
			m_Handler->OnAfterReload(root);
	}

	bool Application::IsAuthenticated(uint16_t userId)
	{
		return m_ApplicationManager->IsAuthenticated(userId);
	}
}