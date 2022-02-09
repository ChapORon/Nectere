#include "Command/CreateCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		CreateCanalCommand::CreateCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_CREATE_CANAL, "create-canal", canalManager) {}
		void CreateCanalCommand::Treat(uint16_t sessionId, uint16_t, const std::string &data)
		{
			if (IsAuthenticated(sessionId))
				m_CanalManager->CreateCanal(sessionId, data);
			else
				SendError(sessionId, "Cannot create canal: User is not authenticated");
		}
	}
}