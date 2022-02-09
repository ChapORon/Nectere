#include "UserManager.hpp"

#include "ApplicationManager.hpp"
#include "AUser.hpp"
#include "CanalManager.hpp"
#include "NectereEventCode.hpp"
#include "VersionHelper.hpp"

namespace Nectere
{
	UserManager::UserManager(const std::string &authKey) : m_AuthenticationKey(authKey)
	{
		m_ApplicationManager = new ApplicationManager(this);
		m_CanalUser = new CanalManager(this);
	}

	AUser *UserManager::InternalAddUser(AUser *user)
	{
		if (user)
		{
			Log(LogType::Standard, '[', user->GetID(), "] User added");
			user->InternalInit(m_IDGenerator.GenerateID(), this);
			m_Users.Add(user);
		}
		return user;
	}

	void UserManager::AuthenticateUser(uint16_t userId, const std::string &authenticateKey)
	{
		if (Ptr<AUser> &user = m_Users.Get(userId))
		{
			Event event;
			event.m_ApplicationID = 0;
			event.m_EventCode = NECTERE_EVENT_AUTH;
			VersionHelper::SetupEvent(event);
			if (authenticateKey == m_AuthenticationKey)
			{
				user->Authenticate();
				event.m_Error = false;
				event.m_Data = "You've been successfully authenticated";
				Log(LogType::Standard, "User ", userId, " authenticated");
			}
			else
			{
				event.m_Error = true;
				event.m_Data = "Authentication failed";
			}
			user->Receive(event);
		}
	}

	void UserManager::RemoveUserInstant(uint16_t id)
	{
		if (m_Users.Remove(id))
		{
			m_IDGenerator.FreeID(id);
			if (m_CanalUser)
				m_CanalUser->LeaveAllCanal(id);
			Log(LogType::Standard, '[', id, "] User removed");
		}
	}

	void UserManager::RemoveUser(uint16_t id)
	{
		m_UsersToRemove.emplace_back(id);
	}

	void UserManager::Update()
	{
		for (uint16_t userToRemove : m_UsersToRemove)
			RemoveUserInstant(userToRemove);
		m_UsersToRemove.clear();
		for (auto user : m_Users)
			user->Update();
		if (m_ApplicationManager)
			m_ApplicationManager->Update();
		if (m_CanalUser)
			m_CanalUser->Update();
	}

	void UserManager::ReceiveEvent(uint16_t userID, const Event &event)
	{
		if (m_ApplicationManager)
			m_ApplicationManager->Receive(userID, event);
	}

	void UserManager::SendEvent(uint16_t id, const Event &event)
	{
		if (Ptr<AUser> &user = m_Users.Get(id))
			user->Receive(event);
	}

	void UserManager::SendEvent(const std::vector<uint16_t> &usersId, const Event &event)
	{
		for (uint16_t id : usersId)
			SendEvent(id, event);
	}

	bool UserManager::IsAuthenticated(uint16_t id)
	{
		if (Ptr<AUser> &user = m_Users.Get(id))
			return user->IsAuthenticated();
		return false;
	}

	void UserManager::SetLogger(const Logger *logger)
	{
		m_Logger = logger;
		if (m_ApplicationManager)
			m_ApplicationManager->SetLogger(logger);
		if (m_CanalUser)
			m_CanalUser->SetLogger(logger);
	}

	UserManager::~UserManager()
	{
		if (m_ApplicationManager)
			delete(m_ApplicationManager);
		if (m_CanalUser)
			delete(m_CanalUser);
	}
}