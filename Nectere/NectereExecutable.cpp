#include "NectereExecutable.hpp"

#include "ApplicationManager.hpp"
#include "Command/StopCommand.hpp"
#include "Concurrency/ThreadSystem.hpp"
#include "Configuration.hpp"
#include "Parameters/LogPathParameter.hpp"
#include "Parameters/NetworkPortParameter.hpp"
#include "Parameters/VerboseParameter.hpp"
#include "Network/Server.hpp"
#include "UserManager.hpp"

namespace Nectere
{
	void NectereExecutable::InitConfiguration()
	{
		//Log
		Configuration::Add<Parameters::VerboseParameter>();
		Configuration::Add<Parameters::LogInFileParameter>();
		Configuration::Add<Parameters::LogPathParameter>(dynamic_cast<Parameters::LogInFileParameter *>(Configuration::Fetch("LogInFile")));

		//Network
		Configuration::Add<Parameters::NetworkPortParameter>();

		//Script Engine
		Configuration::Add<Configuration::BoolParameter>("ScriptEngine.DoSmartBuild", true);
		Configuration::Add<Configuration::BoolParameter>("ScriptEngine.DoCodingStyle", false);
		Configuration::Add<Configuration::BoolParameter>("ScriptEngine.DisplayName", false);
		Configuration::Add<Configuration::BoolParameter>("ScriptEngine.DisplayTag", false);
	}

	void NectereExecutable::Start(int argc, char **argv)
	{
		if (m_ThreadSystem = new Concurrency::ThreadSystem())
		{
			m_ThreadSystem->Start();
			InitConfiguration();
			if (m_UserManager = new UserManager())
			{
				if (auto applicationManager = m_UserManager->GetApplicationManager())
				{
					if (Configuration::LoadConfiguration(argc, argv))
					{
						if (m_Server = Network::MakeServer(Configuration::Get<int>("Network.Port"), m_ThreadSystem, m_UserManager))
						{
							if (auto application = applicationManager->CreateNewApplication("Nectere"))
							{
								application->AddCommand<Command::StopCommand>(m_Server, m_ThreadSystem);
							}
							applicationManager->LoadApplications();
							m_ThreadSystem->AddTask([=]() { return Update(); });
							if (m_Server->Start())
								m_ThreadSystem->Await();
						}
					}
				}
			}
		}
	}

	Concurrency::TaskResult NectereExecutable::Update()
	{
		m_UserManager->Update();
		return Nectere::Concurrency::TaskResult::NeedUpdate;
	}

	void NectereExecutable::Stop()
	{
		if (m_Server)
			m_Server->Stop();
		if (m_ThreadSystem)
			m_ThreadSystem->Stop();
	}

	NectereExecutable::~NectereExecutable()
	{
		if (m_Server)
			delete (m_Server);
		if (m_UserManager)
			delete(m_UserManager);
		if (m_ThreadSystem)
			delete(m_ThreadSystem);
		Configuration::Clear();
	}
}