#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
    #include <csignal>
#endif

#include "NectereExecutable.hpp"

Nectere::NectereExecutable *g_NectereServer = nullptr;

#ifdef WIN32
	static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
	{
		switch (fdwCtrlType)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		{
			g_NectereServer->Stop();
			return TRUE;
		}
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
		{
			g_NectereServer->Stop();
			return FALSE;
		}
		default:
			return FALSE;
		}
	}
#else
	static void SigInterruptHandler(int)
	{
		g_NectereServer->Stop();
	}
#endif

int main(int argc, char **argv)
{
	if (g_NectereServer = new Nectere::NectereExecutable())
	{
		#ifdef WIN32
			if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
				return -1;
		#else
			::signal(SIGINT, SigInterruptHandler);
		#endif
		g_NectereServer->Start(argc, argv);
		delete(g_NectereServer);
		return 0;
	}
	return 1;
}
