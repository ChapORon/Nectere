#include "ApplicationManager.hpp"

#include <filesystem>
#include "Command/StopCommand.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"
#include "UserManager.hpp"

typedef const char *FUNCTION(ApplicationName)();
typedef void FUNCTION(ApplicationLoader)(Nectere::Ptr<Nectere::Application>);

namespace Nectere
{
	ApplicationManager::ApplicationManager(UserManager *userManager): m_UserManager(userManager) {}

	void ApplicationManager::Receive(uint16_t sessionId, const Event &message)
	{
		if (Ptr<Application> &application = m_Applications.Get(message.m_ApplicationID))
		{
			bool eventExist = false;
			LOG(LogType::Verbose, "Checking if application \"", application->GetName(), "\" can receive and treat event with code ", message.m_EventCode);
			if (application->IsEventAllowed(message, eventExist))
			{
				LOG(LogType::Verbose, "Treating event");
				application->Treat(sessionId, message);
			}
			else
				SendEvent(sessionId, Event{ message.m_ApplicationID, message.m_EventCode, (eventExist) ? "Misformated data" : "No such event" });
		}
		else
			SendEvent(sessionId, Event{ message.m_ApplicationID, message.m_EventCode, "No such application" });
	}

	void ApplicationManager::SendEvent(uint16_t id, const Event &event)
	{
		m_UserManager->SendEvent(id, event);
	}

	void ApplicationManager::SendEvent(const std::vector<uint16_t> &ids, const Event &event)
	{
		m_UserManager->SendEvent(ids, event);
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
			LOG(LogType::Standard, moduleName, ": Module loaded");
			ApplicationName applicationName = dynamicLibrary->Load<ApplicationName>("Nectere_ApplicationName");
			if (!applicationName)
				return false;
			std::string name = applicationName();
			if (name.empty())
				return false;
			LOG(LogType::Standard, moduleName, ": Application name is ", name);
			ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
			if (!applicationLoader)
				return false;
			LOG(LogType::Standard, moduleName, ": Loading application");
			if (Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), name, Ptr<IApplicationManager>(this)))
			{
				applicationLoader(Ptr<Application>(application));
				uint16_t applicationID = application->GetID();
				m_LoadedLibrary[dynamicLibrary->GetPath()] = applicationID;
				m_LoadedApplication[applicationID] = std::make_pair(dynamicLibrary, application);
				m_Applications.Add(application);
				LOG(LogType::Standard, moduleName, ": Application loaded");
				return true;
			}
		}
// 		else
// 			return ReloadApplication((*lib).second);
		return false;
	}

	bool ApplicationManager::UnloadApplication(uint16_t applicationID)
	{
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
				LOG(LogType::Standard, moduleName, ": Module reloaded");
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
				LOG(LogType::Standard, moduleName, ": New application name is ", name);
				ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
				if (!applicationLoader)
				{
					UnloadApplication(applicationID);
					return false;
				}
				LOG(LogType::Standard, moduleName, ": Reloading application");
				applicationLoader(Ptr<Application>(application));
				LOG(LogType::Standard, moduleName, ": Application reloaded");
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

	ApplicationManager::~ApplicationManager()
	{
		for (const auto &pair : m_LoadedApplication)
			delete(pair.second.first);
	}
}
