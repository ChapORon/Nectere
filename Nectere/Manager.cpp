#include "Manager.hpp"

#include <filesystem>
#include "ApplicationManager.hpp"
#include "CanalManager.hpp"
#include "Command/ApplicationListCommand.hpp"
#include "Command/AuthCommand.hpp"
#include "Command/CommandListCommand.hpp"
#include "Command/CreateCanalCommand.hpp"
#include "Command/DeleteCanalCommand.hpp"
#include "Command/JoinCanalCommand.hpp"
#include "Command/LeaveCanalCommand.hpp"
#include "Command/ListCanalCommand.hpp"
#include "Command/MessageCanalCommand.hpp"
#include "Command/PingCommand.hpp"
#include "Command/ReloadAppCommand.hpp"
#include "Command/StopCommand.hpp"
#include "Command/UnloadAppCommand.hpp"
#include "Configuration.hpp"
#include "UserManager.hpp"
#include "VersionHelper.hpp"

namespace Nectere
{
	bool Manager::Start(int argc, char **argv)
	{
		VersionHelper::SetVersion("0.1.3");
		if (m_Logger = new Logger("[{d}-{M}-{y} {h}:{m}:{s}.{ms}] {log_type}{msg}"))
		{
			if (m_ThreadSystem)
			{
				if (m_Configuration.LoadConfiguration(argc, argv))
				{
					m_Logger->Init(m_Configuration.Is("Verbose", true), m_Configuration.Is("LogFile", true), m_Configuration.Get<std::string>("LogPath"));
					m_ThreadSystem->SetLogger(m_Logger);
					m_ThreadSystem->Start();
					if (m_UserManager = new UserManager(m_Configuration.Get<std::string>("AuthKey")))
					{
						m_UserManager->SetLogger(m_Logger);
						if (auto applicationManager = m_UserManager->GetApplicationManager())
						{
							if (auto application = applicationManager->CreateBaseApplication())
							{
								application->AddCommand<Command::AuthCommand>(m_UserManager);
								application->AddCommand<Command::ApplicationListCommand>();
								application->AddCommand<Command::CommandListCommand>();
								application->AddCommand<Command::PingCommand>();
								application->AddCommand<Command::ReloadAppCommand>();
								application->AddCommand<Command::StopCommand>(Ptr<Concurrency::AThreadSystem>(m_ThreadSystem));
								application->AddCommand<Command::UnloadAppCommand>();
								if (auto canalManager = m_UserManager->GetCanalManager())
								{
									canalManager->InitConfiguration(m_Configuration);
									application->AddCommand<Command::CreateCanalCommand>(canalManager);
									application->AddCommand<Command::DeleteCanalCommand>(canalManager);
									application->AddCommand<Command::JoinCanalCommand>(canalManager);
									application->AddCommand<Command::LeaveCanalCommand>(canalManager);
									application->AddCommand<Command::ListCanalCommand>(canalManager);
									application->AddCommand<Command::MessageCanalCommand>(canalManager);
								}
							}
							try
							{
								for (const auto &entry : std::filesystem::directory_iterator("plugins"))
								{
									if (!entry.is_directory())
										applicationManager->LoadApplication(entry.path().string());
								}
							}
							catch (const std::exception &) {}
							m_ThreadSystem->AddTask([=]()
								{
									m_UserManager->Update();
									return Concurrency::TaskResult::NeedUpdate;
								});
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	void Manager::Await()
	{
		if (m_ThreadSystem)
			m_ThreadSystem->Await();
	}

	void Manager::Stop()
	{
		if (m_ThreadSystem)
			m_ThreadSystem->Stop();
	}

	Manager::~Manager()
	{
		if (m_UserManager)
			delete(m_UserManager);
		if (m_ThreadSystem)
			delete(m_ThreadSystem);
		m_Configuration.Clear();
		if (m_Logger)
			delete(m_Logger);
	}
}