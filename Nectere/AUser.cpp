#include "AUser.hpp"

#include "UserManager.hpp"

namespace Nectere
{
	void AUser::InternalInit(uint16_t id, UserManager *userManager)
	{
		m_ID = id;
		m_UserManager = userManager;
		OnInit();
	}

	void AUser::Send(const Event &event)
	{
		if (m_UserManager)
			m_UserManager->ReceiveEvent(m_ID, event);
	}
}