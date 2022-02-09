#include "AUser.hpp"

#include "NectereEventCode.hpp"
#include "UserManager.hpp"
#include "VersionHelper.hpp"

namespace Nectere
{
	void AUser::InternalInit(uint16_t id, UserManager *userManager)
	{
		m_ID = id;
		m_UserManager = userManager;
		OnInit();
		Event event;
		event.m_ApplicationID = 0;
		event.m_CanalID = 0;
		event.m_UserID = id;
		event.m_EventCode = NECTERE_EVENT_USER_CONNECTED;
		event.m_Error = false;
		VersionHelper::SetupEvent(event);
		Receive(event);
	}

	void AUser::Send(const Event &event)
	{
		if (m_UserManager)
			m_UserManager->ReceiveEvent(event.m_UserID, event);
	}

	void AUser::Close()
	{
		OnClose();
	}

	void AUser::MarkForDelete()
	{
		if (m_UserManager)
			m_UserManager->RemoveUser(m_ID);
		//m_Handler->CloseSession(GetID()); #todo
	}
}