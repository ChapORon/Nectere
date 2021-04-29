#include <iostream>
#include "Configuration.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Dp/Json.hpp"
#include "Dp/Xml.hpp"

#include "parg.h"

namespace Nectere
{
	static void SetPort(const char *value)
	{
		try
		{
			Configuration::Set("Network.Port", std::stoi(value));
			LOG(LogType::Standard, "Setting configuration port to ", Configuration::GetInt("Network.Port"));
		}
		catch (const std::exception &) {}
	}

	static void Init()
	{

		//Script Engine
		#ifndef DEBUG
			Configuration::Set("Verbose", false);
		#else
			Configuration::Set("Verbose", true);
		#endif
		
		//Network
		Configuration::Set("Network.Port", 4242);

		//Script Engine
		Configuration::Set("ScriptEngine.DoSmartBuild", true);
		Configuration::Set("ScriptEngine.DoConsoleOutput", true);
		Configuration::Set("ScriptEngine.DoCodingStyle", false);
		Configuration::Set("ScriptEngine.DefaultCodingStyle", false);
		Configuration::Set("ScriptEngine.DisplayName", false);
		Configuration::Set("ScriptEngine.DisplayTag", false);
	}
}

int main(int argc, char **argv)
{
	Nectere::Init();
	parg *root = parg_alloc();
	parg_add_vcallback(root, "port", &Nectere::SetPort, 2);
	parg_add_vcallback(root, "p", &Nectere::SetPort, 1);
	parg_parse(root, argc, argv);
	parg_free(root);
	Nectere::Server server;
    server.LoadApplications();
 	//server.Start();
	return 0;
}
