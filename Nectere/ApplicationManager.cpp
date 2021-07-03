#include "ApplicationManager.hpp"

#include "Command/StopCommand.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"
#include "ThreadSystem.hpp"

typedef const char *FUNCTION(ApplicationName)();
typedef void FUNCTION(ApplicationLoader)(Nectere::Application *);

namespace Nectere
{
	void ApplicationManager::OnReceive(uint16_t sessionId, const Event &message)
	{
		LOG(LogType::Verbose, "Server created on port ", Configuration::Get<int>("Network.Port"));
		if (Application *application = m_Applications.Get(message.m_ApplicationID))
		{
			LOG(LogType::Verbose, "Checking if application \"", application->GetName(), "\" can receive and treat event with code ", message.m_EventCode);
			if (application->IsEventAllowed(message))
			{
				LOG(LogType::Verbose, "Treating event");
				application->Treat(sessionId, message);
			}
		}
	}

	void ApplicationManager::GenerateNectereApplication(Network::AServer *server, ThreadSystem *threadSystem)
	{
		Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), "Nectere");
		application->AddCommand(new Command::StopCommand(server, threadSystem));
		m_Applications.Add(application);
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
			Application *application = new Application(m_ApplicationIDGenerator.GenerateID(), name);
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
				if (!dynamicLibrary)
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

	TaskResult ApplicationManager::Update()
	{
		for (const auto &application : m_Applications.GetElements())
			application->Update();
		return TaskResult::NeedUpdate;
	}

	void ApplicationManager::LoadApplications(Network::AServer *server, ThreadSystem *threadSystem)
	{
		GenerateNectereApplication(server, threadSystem);
		try
		{
			for (const auto &entry : std::filesystem::directory_iterator("plugins"))
			{
				if (!entry.is_directory())
					LoadApplication(entry.path());
			}
			threadSystem->AddTask(this, &ApplicationManager::Update);
		}
		catch (const std::exception &) {}
	}

	ApplicationManager::~ApplicationManager()
	{
		for (const auto &pair : m_LoadedLibrary)
			delete(pair.second.first);
	}
}
