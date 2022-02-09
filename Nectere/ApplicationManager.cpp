#include "ApplicationManager.hpp"

#include <filesystem>
#include "Command/StopCommand.hpp"
#include "Logger.hpp"
#include "UserManager.hpp"

typedef const char *FUNCTION(ApplicationName)();
typedef void FUNCTION(ApplicationLoader)(Nectere::Ptr<Nectere::Application>);

namespace Nectere
{
	ApplicationManager::ApplicationManager(const Ptr<UserManager> &userManager): m_UserManager(userManager), m_ApplicationIDGenerator(1) {}

	void ApplicationManager::Receive(uint16_t sessionId, const Event &message)
	{
		if (Ptr<Application> &application = m_Applications.Get(message.m_ApplicationID))
		{
			bool eventExist = false;
			Log(LogType::Verbose, "Checking if application \"", application->GetName(), "\" can receive and treat event with code ", message.m_EventCode);
			if (application->IsEventAllowed(message, eventExist))
			{
				Log(LogType::Verbose, "Treating event");
				application->Treat(sessionId, message);
			}
			else
			{
				Event event = message;
				event.m_Error = true;
				event.m_Data = (eventExist) ? "Misformated data" : "No such event";
				SendEvent(sessionId, event);
			}
		}
		else
		{
			Event event = message;
			event.m_Error = true;
			event.m_Data = "No such application";
			SendEvent(sessionId, event);
		}
	}

	void ApplicationManager::SendEvent(uint16_t id, const Event &event)
	{
		m_UserManager->SendEvent(id, event);
	}

	void ApplicationManager::SendEvent(const std::vector<uint16_t> &ids, const Event &event)
	{
		m_UserManager->SendEvent(ids, event);
	}

	Ptr<Application> ApplicationManager::CreateBaseApplication()
	{
		Application *application = new Application(0, "Nectere", Ptr<IApplicationManager>(this));
		m_Applications.Add(application);
		return Ptr(application);
	}

    Ptr<Application> ApplicationManager::CreateNewApplication(const std::string &applicationName)
    {
        Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), applicationName, Ptr<IApplicationManager>(this));
        m_Applications.Add(application);
        return Ptr(application);
    }

	bool ApplicationManager::LoadApplication(const std::string &path)
	{
		std::filesystem::path absolutePath = std::filesystem::absolute(path);
		std::string absolutePathStr = absolutePath.string();
		std::string moduleName = absolutePath.filename().string();
		auto lib = m_LoadedLibrary.find(absolutePathStr);
		if (lib == m_LoadedLibrary.end())
		{
			DynamicLibrary *dynamicLibrary = new DynamicLibrary(absolutePathStr);
			if (!dynamicLibrary || !(*dynamicLibrary))
				return false;
			Log(LogType::Standard, moduleName, ": Module loaded");
			ApplicationName applicationName = dynamicLibrary->Load<ApplicationName>("Nectere_ApplicationName");
			if (!applicationName)
				return false;
			std::string name = applicationName();
			if (name.empty())
				return false;
			Log(LogType::Standard, moduleName, ": Application name is ", name);
			ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
			if (!applicationLoader)
				return false;
			Log(LogType::Standard, moduleName, ": Loading application");
			if (Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), name, Ptr<IApplicationManager>(this)))
			{
				applicationLoader(Ptr<Application>(application));
				uint16_t applicationID = application->GetID();
				m_LoadedLibrary[dynamicLibrary->GetPath()] = applicationID;
				m_LoadedApplication[applicationID] = std::make_pair(dynamicLibrary, application);
				m_Applications.Add(application);
				Log(LogType::Standard, moduleName, ": Application loaded");
				return true;
			}
		}
// 		else
// 			return ReloadApplication((*lib).second);
		return false;
	}

	bool ApplicationManager::UnloadApplication(uint16_t applicationID)
	{
		if (applicationID == 0)
			return false;
		auto it = m_LoadedApplication.find(applicationID);
		if (it != m_LoadedApplication.end())
		{
			DynamicLibrary *dynamicLibrary = (*it).second.first;
			m_LoadedLibrary.erase(m_LoadedLibrary.find(dynamicLibrary->GetPath()));
			m_LoadedApplication.erase(it);
			m_ApplicationIDGenerator.FreeID(applicationID);
			m_Applications.Remove(applicationID);
			return true;
		}
		return false;
	}

	bool ApplicationManager::ReloadApplication(uint16_t applicationID)
	{
		if (applicationID == 0)
			return false;
		auto it = m_LoadedApplication.find(applicationID);
		if (it != m_LoadedApplication.end())
		{
			DynamicLibrary *dynamicLibrary = (*it).second.first;
			Application *application = (*it).second.second;
			if (dynamicLibrary->CanReload())
			{
				Dp::Node root;
				application->BeforeReloading(root);
				dynamicLibrary->Reload();
				if (!(*dynamicLibrary))
				{
					UnloadApplication(applicationID);
					return false;
				}
				std::string moduleName = std::filesystem::path(dynamicLibrary->GetPath()).filename().string();
				Log(LogType::Standard, moduleName, ": Module reloaded");
				ApplicationName applicationName = dynamicLibrary->Load<ApplicationName>("Nectere_ApplicationName");
				if (!applicationName)
				{
					UnloadApplication(applicationID);
					return false;
				}
				std::string name = applicationName();
				if (name.empty())
				{
					UnloadApplication(applicationID);
					return false;
				}
				application->SetName(name);
				Log(LogType::Standard, moduleName, ": New application name is ", name);
				ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
				if (!applicationLoader)
				{
					UnloadApplication(applicationID);
					return false;
				}
				Log(LogType::Standard, moduleName, ": Reloading application");
				applicationLoader(Ptr<Application>(application));
				Log(LogType::Standard, moduleName, ": Application reloaded");
				application->AfterReloading(root);
				return true;
			}
		}
		return false;
	}

    void ApplicationManager::Update()
    {
        for (auto application : m_Applications)
            application->Update();
	}

	bool ApplicationManager::IsAuthenticated(uint16_t userId)
	{
		return m_UserManager->IsAuthenticated(userId);
	}

	ApplicationManager::~ApplicationManager()
	{
		for (const auto &pair : m_LoadedApplication)
			delete(pair.second.first);
	}
}
