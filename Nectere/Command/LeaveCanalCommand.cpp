#include "Command/LeaveCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		LeaveCanalCommand::LeaveCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_LEAVE_CANAL, "leave-canal", canalManager) {}
		void LeaveCanalCommand::Treat(uint16_t sessionId, uint16_t canalId, const std::string &) { m_CanalManager->LeaveCanal(sessionId, canalId); }
	}
}