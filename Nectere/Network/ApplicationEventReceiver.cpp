#include "Network/ApplicationEventReceiver.hpp"

#include "ApplicationManager.hpp"

namespace Nectere
{
	namespace Network
	{
		ApplicationEventReceiver::ApplicationEventReceiver(ApplicationManager *applicationManager): m_ApplicationManager(applicationManager) {}

		void ApplicationEventReceiver::OnReceive(uint16_t sessionID, const Event &event)
		{
			if (m_ApplicationManager)
				m_ApplicationManager->Receive(sessionID, event);
		}
	}
}