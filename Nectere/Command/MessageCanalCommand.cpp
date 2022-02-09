#include "Command/MessageCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		MessageCanalCommand::MessageCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_MSG_CANAL, "msg-canal", canalManager) {}
		void MessageCanalCommand::Treat(uint16_t sessionId, uint16_t canalId, const std::string &data) { m_CanalManager->MessageCanal(sessionId, canalId, data); }
	}
}