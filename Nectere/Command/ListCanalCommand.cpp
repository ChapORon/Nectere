#include "Command/ListCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		ListCanalCommand::ListCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_LIST_CANAL, "list-canal", canalManager) {}
		void ListCanalCommand::Treat(uint16_t sessionId, uint16_t, const std::string &) { m_CanalManager->ListCanal(sessionId); }
	}
}