#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
    #include <csignal>
#endif

#include "Concurrency/ThreadSystem.hpp"
#include "Manager.hpp"
#include "Network/Server.hpp"
#include "Network/NetworkPortParameter.hpp"
#include "Network/NetworkWhitelistParameter.hpp"

Nectere::Manager *g_NectereManager = nullptr;

#ifdef WIN32
	static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
	{
		switch (fdwCtrlType)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		{
			g_NectereManager->Stop();
			return TRUE;
		}
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			g_NectereManager->Stop();
			return FALSE;
		}
		default:
			return FALSE;
		}
	}
#else
	static void SigInterruptHandler(int)
	{
		g_NectereManager->Stop();
	}
#endif

int main(int argc, char **argv)
{
	if (g_NectereManager = new Nectere::Manager())
	{
		#ifdef WIN32
			if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
				return -1;
		#else
			::signal(SIGINT, SigInterruptHandler);
		#endif
		if (g_NectereManager->SetThreadSystem<Nectere::Concurrency::ThreadSystem>())
		{
			g_NectereManager->GetConfiguration().Add<Nectere::Network::NetworkPortParameter>();
			g_NectereManager->GetConfiguration().Add<Nectere::Network::NetworkWhitelistParameter>();
			if (g_NectereManager->Start(argc, argv))
			{
				Nectere::Concurrency::AThreadSystem *threadSystem = g_NectereManager->GetThreadSystem();
				if (Nectere::Network::AServer *server = Nectere::Network::MakeServer(g_NectereManager->GetConfiguration().Get<int>("Network.Port"),
					g_NectereManager->GetConfiguration().Get<std::string>("Network.Whitelist"),
					threadSystem, g_NectereManager->GetLogger(), g_NectereManager->GetUserManager()))
				{
					threadSystem->SetOnStopCallback([=]() { server->Stop(); });
					if (server->Start())
						threadSystem->Await();
					delete (server);
				}
			}
		}
		delete(g_NectereManager);
		return 0;
	}
	return 1;
}
