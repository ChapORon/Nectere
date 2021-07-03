#include <csignal>
#include <iostream>
#include "ApplicationManager.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"
#include "Parameters/LogPathParameter.hpp"
#include "Parameters/NetworkPortParameter.hpp"
#include "Parameters/VerboseParameter.hpp"
#include "Network/Server.hpp"
#include "ThreadSystem.hpp"

Nectere::ThreadSystem *g_threadSystem = nullptr;
Nectere::Network::AServer *g_Server = nullptr;

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
			if (g_threadSystem)
				g_threadSystem->Stop();
			return TRUE;
		}
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			if (g_Server)
				g_Server->Stop();
			if (g_threadSystem)
				g_threadSystem->Stop();
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
		if (g_threadSystem)
			g_threadSystem->Stop();
	}
#endif

int main(int argc, char **argv)
{
	#ifdef WIN32
		if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
			return -1;
	#else
		::signal(SIGINT, SigInterruptHandler);
	#endif
	if (g_threadSystem = new Nectere::ThreadSystem())
	{
		g_threadSystem->Start();
		Nectere::Init();
		Nectere::ApplicationManager applicationManager;
		if (Nectere::Configuration::LoadConfiguration(argc, argv))
		{
			if (g_Server = Nectere::Network::MakeServer(Nectere::Configuration::Get<int>("Network.Port"), g_threadSystem, &applicationManager))
			{
				applicationManager.LoadApplications(g_Server, g_threadSystem);
				if (g_Server->Start())
					g_threadSystem->Await();
				delete(g_Server);
			}
		}
		delete(g_threadSystem);
		Nectere::Configuration::Clear();
		return 0;
	}
	return 1;
}
