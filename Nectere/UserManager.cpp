#include "UserManager.hpp"

#include "ApplicationManager.hpp"
#include "AUser.hpp"
#include "Logger.hpp"

namespace Nectere
{
	UserManager::UserManager()
	{
		m_ApplicationManager = new ApplicationManager(this);
	}

	uint16_t UserManager::InternalAddUser(AUser *user)
	{
		if (user)
		{
			LOG(LogType::Standard, '[', user->GetID(), "] User added");
			user->InternalInit(m_IDGenerator.GenerateID(), this);
			m_Users.Add(user);
			return user->GetID();
		}
		return 0;
	}

	void UserManager::RemoveUser(uint16_t id)
	{
		if (m_Users.Remove(id))
			LOG(LogType::Standard, '[', id, "] User removed");
	}

	void UserManager::Update()
	{
		if (m_ApplicationManager)
			m_ApplicationManager->Update();
	}

	void UserManager::ReceiveEvent(uint16_t userID, const Event &event)
	{
		for (auto user : m_Users.GetElements())
			user->Update();
		if (m_ApplicationManager)
			m_ApplicationManager->Receive(userID, event);
	}

	void UserManager::SendEvent(uint16_t id, const Event &event)
	{
		if (AUser *user = m_Users.Get(id))
			user->Receive(event);
	}

	void UserManager::SendEvent(const std::vector<uint16_t> &usersId, const Event &event)
	{
		for (uint16_t id : usersId)
			SendEvent(id, event);
	}

	UserManager::~UserManager()
	{
		if (m_ApplicationManager)
			delete(m_ApplicationManager);
	}
}