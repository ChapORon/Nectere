#include "ApplicationManager.hpp"

#include "Command/StopCommand.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"
#include "UserManager.hpp"

typedef const char *FUNCTION(ApplicationName)();
typedef void FUNCTION(ApplicationLoader)(Nectere::Application *);

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

	void ApplicationManager::LoadApplication(const std::filesystem::path &path)
	{
		std::string absolutePath = std::filesystem::absolute(path).string();
		std::string moduleName = path.filename().string();
		auto lib = m_LoadedLibrary.find(absolutePath);
		if (lib == m_LoadedLibrary.end())
		{
			DynamicLibrary *dynamicLibrary = new DynamicLibrary(absolutePath);
			if (!dynamicLibrary)
				return;
			LOG(LogType::Standard, moduleName, ": Module loaded");
			ApplicationName applicationName = dynamicLibrary->Load<ApplicationName>("Nectere_ApplicationName");
			if (!applicationName)
				return;
			std::string name = applicationName();
			if (name.empty())
				return;
			LOG(LogType::Standard, moduleName, ": Application name is ", name);
			ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
			if (!applicationLoader)
				return;
			LOG(LogType::Standard, moduleName, ": Loading application");
			Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), name, Ptr<IApplicationManager>(this));
			applicationLoader(application);
			m_LoadedLibrary[dynamicLibrary->GetPath()] = std::make_pair(dynamicLibrary, application);
			m_Applications.Add(application);
			LOG(LogType::Standard, moduleName, ": Application loaded");
		}
		else
		{
			DynamicLibrary *dynamicLibrary = (*lib).second.first;
			Application *application = (*lib).second.second;
			if (dynamicLibrary->CanReload())
			{
				dynamicLibrary->Reload();
				if (!static_cast<bool>(dynamicLibrary))
				{
					uint16_t applicationID = application->GetID();
					m_ApplicationIDGenerator.FreeID(applicationID);
					m_Applications.Remove(applicationID);
					m_LoadedLibrary.erase(lib);
					return;
				}
				application->BeforeReloading();
				LOG(LogType::Standard, moduleName, ": Module reloaded");
				ApplicationName applicationName = dynamicLibrary->Load<ApplicationName>("Nectere_ApplicationName");
				if (!applicationName)
					return;
				std::string name = applicationName();
				if (name.empty())
					return;
				application->SetName(name);
				LOG(LogType::Standard, moduleName, ": New application name is ", name);
				ApplicationLoader applicationLoader = dynamicLibrary->Load<ApplicationLoader>("Nectere_ApplicationLoader");
				if (!applicationLoader)
					return;
				LOG(LogType::Standard, moduleName, ": Reloading application");
				applicationLoader(application);
				LOG(LogType::Standard, moduleName, ": Application reloaded");
				application->AfterReloading();
			}
		}
	}

    void ApplicationManager::Update()
    {
        for (auto application : m_Applications)
            application->Update();
    }

	void ApplicationManager::LoadApplications()
	{
		try
		{
			for (const auto &entry : std::filesystem::directory_iterator("plugins"))
			{
				if (!entry.is_directory())
					LoadApplication(entry.path());
			}
		}
		catch (const std::exception &) {}
	}

	ApplicationManager::~ApplicationManager()
	{
		for (const auto &pair : m_LoadedLibrary)
			delete(pair.second.first);
	}
}
