#include "Command/JoinCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		JoinCanalCommand::JoinCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_JOIN_CANAL, "join-canal", canalManager) {}
		void JoinCanalCommand::Treat(uint16_t sessionId, uint16_t canalId, const std::string &data) { m_CanalManager->JoinCanal(sessionId, canalId, data); }
	}
}