#include "Command/AuthCommand.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "UserManager.hpp"

namespace Nectere
{
	namespace Command
	{
		AuthCommand::AuthCommand(const Ptr<UserManager> &userManager) : ACommand(NECTERE_EVENT_AUTH, "auth"), m_UserManager(userManager) {}
		void AuthCommand::Treat(uint16_t sessionId, uint16_t, const std::string &data) { m_UserManager->AuthenticateUser(sessionId, data); }
	}
}