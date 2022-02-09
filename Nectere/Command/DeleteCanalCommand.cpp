#include "Command/DeleteCanalCommand.hpp"

#include "CanalManager.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"

namespace Nectere
{
	namespace Command
	{
		DeleteCanalCommand::DeleteCanalCommand(const Ptr<CanalManager> &canalManager) : ACanalCommand(NECTERE_EVENT_DELETE_CANAL, "delete-canal", canalManager) {}
		void DeleteCanalCommand::Treat(uint16_t sessionId, uint16_t canalId, const std::string &)\
		{
			if (IsAuthenticated(sessionId))
				m_CanalManager->DeleteCanal(sessionId, canalId);
			else
				SendError(sessionId, "Cannot delete canal: User is not authenticated");
		}
	}
}