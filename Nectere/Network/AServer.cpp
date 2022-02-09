#include "AServer.hpp"

#include <fstream>

namespace Nectere
{
	namespace Network
	{
		AServer::AServer(int port, const std::string &whitelistFile, Concurrency::AThreadSystem *threadSystem, const Logger *logger, UserManager *userManager) :
			m_WhitelistEnable(!whitelistFile.empty()), m_Port(port), m_ThreadSystem(threadSystem), m_Logger(logger), m_UserManager(userManager)
		{
			if (!whitelistFile.empty())
			{
				std::ifstream file(whitelistFile);
				if (file)
				{
					std::string str;
					while (std::getline(file, str))
						m_WhitelistedIP.insert(str);
				}
				else
					m_WhitelistEnable = false;
			}
		}

		bool AServer::IsIPWhitelisted(const std::string &ip)
		{
			if (ip == "127.0.0.1" || !m_WhitelistEnable)
				return true;
			return m_WhitelistedIP.find(ip) != m_WhitelistedIP.end();
		}
	}
}