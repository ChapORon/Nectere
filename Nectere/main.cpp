#ifndef WIN32
    #include <csignal>
#endif
#include "ApplicationManager.hpp"
#include "Command/StopCommand.hpp"
#include "Concurrency/ThreadSystem.hpp"
#include "Configuration.hpp"
#include "Parameters/LogPathParameter.hpp"
#include "Parameters/NetworkPortParameter.hpp"
#include "Parameters/VerboseParameter.hpp"
#include "Network/ApplicationEventReceiver.hpp"
#include "Network/Server.hpp"
#include "UserManager.hpp"

Nectere::Concurrency::ThreadSystem *g_ThreadSystem = nullptr;
Nectere::Network::AServer *g_Server = nullptr;
Nectere::UserManager *g_UserManager = nullptr;

namespace Nectere
{
	static void Init()
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
}

#ifdef WIN32
	static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
	{
		switch (fdwCtrlType)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		{
			if (g_Server)
				g_Server->Stop();
			if (g_ThreadSystem)
                g_ThreadSystem->Stop();
			return TRUE;
		}
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			if (g_Server)
				g_Server->Stop();
			if (g_ThreadSystem)
                g_ThreadSystem->Stop();
			return FALSE;
		}
		default:
			return FALSE;
		}
	}
#else
	static void SigInterruptHandler(int)
	{
		if (g_Server)
			g_Server->Stop();
		if (g_ThreadSystem)
			g_ThreadSystem->Stop();
	}
#endif

Nectere::Concurrency::TaskResult UpdateApplicationManager()
{
	g_UserManager->Update();
    return Nectere::Concurrency::TaskResult::NeedUpdate;
}

int main(int argc, char **argv)
{
	#ifdef WIN32
		if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
			return -1;
	#else
		::signal(SIGINT, SigInterruptHandler);
    #endif
	if (g_ThreadSystem = new Nectere::Concurrency::ThreadSystem())
	{
        g_ThreadSystem->Start();
		Nectere::Init();
        if (g_UserManager = new Nectere::UserManager())
        {
			if (Nectere::ApplicationManager *applicationManager = g_UserManager->GetApplicationManager())
			{
				if (Nectere::Configuration::LoadConfiguration(argc, argv))
				{
					if (g_Server = Nectere::Network::MakeServer(Nectere::Configuration::Get<int>("Network.Port"), g_ThreadSystem, g_UserManager))
					{
						Nectere::Application *application = applicationManager->CreateNewApplication("Nectere");
						application->AddCommand(new Nectere::Command::StopCommand(g_Server, g_ThreadSystem));
						applicationManager->LoadApplications();
						g_ThreadSystem->AddTask(&UpdateApplicationManager);
						if (g_Server->Start())
							g_ThreadSystem->Await();
						delete (g_Server);
					}
				}
			}
            delete(g_UserManager);
        }
		delete(g_ThreadSystem);
		Nectere::Configuration::Clear();
		return 0;
	}
	return 1;
}
