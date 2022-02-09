#include "CanalManager.hpp"

#include "Dp/Json.hpp"
#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "UserManager.hpp"
#include "VersionHelper.hpp"

namespace Nectere
{
	CanalManager::CanalManager(UserManager *userManager): m_UserManager(userManager) {}

	bool CanalManager::HaveCanal(uint16_t canalID)
	{
		return (m_Canals.find(canalID) != m_Canals.end());
	}

	bool CanalManager::HaveCanal(const std::string &canalName)
	{
		return (m_CanalDictionary.find(canalName) != m_CanalDictionary.end());
	}

	Canal &CanalManager::GetCanal(uint16_t canalID)
	{
		return m_Canals[canalID];
	}

	Canal &CanalManager::GetCanal(const std::string &canalName)
	{
		return GetCanal(m_CanalDictionary[canalName]);
	}

	bool CanalManager::CreateCanal(const std::string &canalName, const std::string &canalPassword, int canalLimit, uint16_t &id)
	{
		if (!HaveCanal(canalName))
		{
			id = m_IDGenerator.GenerateID();
			m_CanalDictionary[canalName] = id;
			m_Canals[id] = Canal(canalName, canalPassword, canalLimit);
			return true;
		}
		return false;
	}

	void CanalManager::DeleteCanal(uint16_t canalID)
	{
		auto it = m_Canals.find(canalID);
		if (it != m_Canals.end())
		{
			for (auto &pair : m_ClientLinks)
			{
				ConnectedClient &client = pair.second;
				auto clientLinkIt = client.m_Links.find(canalID);
				if (clientLinkIt != client.m_Links.end())
				{
					client.m_Links.erase(clientLinkIt);
					SendAnswer(pair.first, canalID, NECTERE_EVENT_CANAL_DELETED, std::to_string(canalID));
				}
			}
			m_CanalDictionary.erase(m_CanalDictionary.find((*it).second.GetName()));
			m_Canals.erase(it);
			m_IDGenerator.FreeID(canalID);
		}
	}

	void CanalManager::InitConfiguration(const Configuration &configuration)
	{
		const Dp::Node &loadedConfig = configuration.GetLoadedConfig();
		if (loadedConfig.IsNotNullNode())
		{
			if (loadedConfig.Find("canal.list"))
			{
				uint16_t canalId;
				const Dp::Node &canalListNode = loadedConfig.GetNode("canal.list");
				for (const Dp::Node &canalNode : canalListNode)
				{
					std::string canalName = canalNode.Get<std::string>("name");
					std::string canalPassword = canalNode.Get<std::string>("password", "");
					int canalLimit = canalNode.Get<int>("limit", -1);
					if (CreateCanal(canalName, canalPassword, canalLimit, canalId))
						Log(LogType::Standard, "Canal ", canalId, " created");
				}
			}
		}
	}

	void CanalManager::Update()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		std::vector<uint16_t> toDelete;

		for (auto &pair : m_Canals)
		{
			Canal &canal = pair.second;
			if (canal.IsMarkForDelete() && canal.IsEmpty())
				toDelete.emplace_back(pair.first);
		}

		for (uint16_t canalId : toDelete)
		{
			DeleteCanal(canalId);
			Log(LogType::Standard, "Canal ", canalId, " deleted");
		}
	}

	void CanalManager::SendAnswer(uint16_t userId, uint16_t canalId, uint16_t eventCode, const std::string &data)
	{
		if (m_UserManager)
		{
			Event event;
			event.m_Error = false;
			event.m_CanalID = canalId;
			event.m_ApplicationID = 0;
			event.m_EventCode = eventCode;
			event.m_Data = data;
			VersionHelper::SetupEvent(event);
			m_UserManager->SendEvent(userId, event);
		}
	}

	void CanalManager::SendError(uint16_t userId, uint16_t canalId, uint16_t eventCode, const std::string &data)
	{
		if (m_UserManager)
		{
			Event event;
			event.m_Error = true;
			event.m_CanalID = canalId;
			event.m_ApplicationID = 0;
			event.m_EventCode = eventCode;
			event.m_Data = data;
			VersionHelper::SetupEvent(event);
			m_UserManager->SendEvent(userId, event);
		}
	}

	void CanalManager::CreateCanal(uint16_t userId, const std::string &canalInfo)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		Dp::Node canalNode = Dp::Json::LoadFromContent(canalInfo);
		std::string canalName = canalNode.Get<std::string>("name");
		std::string canalPassword = canalNode.Get<std::string>("password", "");
		int canalLimit = canalNode.Get<int>("limit", -1);
		uint16_t canalId;
		if (CreateCanal(canalName, canalPassword, canalLimit, canalId))
		{
			std::stringstream ss;
			ss << "Canal created";
			if (!canalPassword.empty())
			{
				ss << " with password";
				if (canalLimit != -1)
					ss << " and a user limit set to " << canalLimit;
			}
			else if (canalLimit != -1)
				ss << " with a user limit set to " << canalLimit;
			SendAnswer(userId, canalId, NECTERE_EVENT_CREATE_CANAL, ss.str());
			Log(LogType::Standard, "Canal ", canalId, " created");
		}
		else
			SendError(userId, canalId, NECTERE_EVENT_CREATE_CANAL, "Cannot create canal: Canal already exist");
	}

	void CanalManager::DeleteCanal(uint16_t userId, uint16_t canalId)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (HaveCanal(canalId))
		{
			Canal &canal = GetCanal(canalId);
			if (canal.IsEmpty())
			{
				DeleteCanal(canalId);
				SendAnswer(userId, canalId, NECTERE_EVENT_DELETE_CANAL, "Canal deleted");
			}
			else
			{
				canal.MarkForDelete();
				SendAnswer(userId, canalId, NECTERE_EVENT_DELETE_CANAL, "Canal marked for deletion");
				Log(LogType::Standard, "Canal ", std::to_string(canalId), " marked for deletion");
			}
		}
		else
			SendError(userId, canalId, NECTERE_EVENT_DELETE_CANAL, "Cannot delete canal: Canal does not exist");
	}

	void CanalManager::JoinCanal(uint16_t userId, uint16_t canalId, const std::string &pass)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (HaveCanal(canalId))
		{
			Canal &canal = GetCanal(canalId);
			if (canal.GetPassword() == pass)
			{
				if (!canal.IsFull())
				{
					canal.AddUser(userId);
					if (m_ClientLinks.find(userId) == m_ClientLinks.end())
						m_ClientLinks[userId] = ConnectedClient();
					m_ClientLinks[userId].m_Links.emplace(canalId);
					SendAnswer(userId, canalId, NECTERE_EVENT_JOIN_CANAL, "Canal joined");
					Log(LogType::Standard, "User ", userId, " joined canal ", canalId);
				}
				else
					SendError(userId, 0, NECTERE_EVENT_JOIN_CANAL, "Cannot join canal: Canal is full");
			}
			else
				SendError(userId, 0, NECTERE_EVENT_JOIN_CANAL, "Cannot join canal: Password is incorrect");
		}
		else
			SendError(userId, 0, NECTERE_EVENT_JOIN_CANAL, "Cannot join canal: Canal does not exist");
	}

	void CanalManager::LeaveCanal(uint16_t userId, uint16_t canalId)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (HaveCanal(canalId))
		{
			Canal &canal = GetCanal(canalId);
			std::string canalName = canal.GetName();
			std::unordered_set<uint16_t> &clientCanals = m_ClientLinks[userId].m_Links;
			auto canalToLeaveIt = clientCanals.find(canalId);
			if (canalToLeaveIt != clientCanals.end())
			{
				canal.RemoveUser(userId);
				clientCanals.erase(canalToLeaveIt);
				if (canal.IsMarkForDelete() && canal.IsEmpty())
					DeleteCanal(canalId);
				SendAnswer(userId, canalId, NECTERE_EVENT_LEAVE_CANAL, "Canal leaved");
				Log(LogType::Standard, "User ", userId, " leaved canal ", canalName);
			}
			else
				SendError(userId, canalId, NECTERE_EVENT_LEAVE_CANAL, "Cannot leave canal: User has not joined it yet");
		}
		else
			SendError(userId, canalId, NECTERE_EVENT_LEAVE_CANAL, "Cannot leave canal: Canal does not exist");
	}

	void CanalManager::LeaveAllCanal(uint16_t clientId)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		auto linkIt = m_ClientLinks.find(clientId);
		if (linkIt != m_ClientLinks.end())
		{
			const std::unordered_set<uint16_t> &links = (*linkIt).second.m_Links;
			for (uint16_t canalId : links)
			{
				if (HaveCanal(canalId))
				{
					Canal &canal = GetCanal(canalId);
					canal.RemoveUser(clientId);
					if (canal.IsMarkForDelete() && canal.IsEmpty())
						DeleteCanal(canalId);
				}
			}
			m_ClientLinks.erase(linkIt);
			Log(LogType::Standard, "User ", clientId, " has left all canals");
		}
	}

	void CanalManager::MessageCanal(uint16_t userId, uint16_t canalId, const std::string &content)
	{
		if (m_UserManager)
		{
			std::vector<uint16_t> users;
			if (HaveCanal(canalId))
			{
				Canal &canal = GetCanal(canalId);
				std::unordered_set<uint16_t> clientToSendTo = canal.GetUsers();
				auto clientIt = clientToSendTo.find(userId);
				if (clientIt != clientToSendTo.end())
					clientToSendTo.erase(clientIt);
				else
				{
					SendError(userId, canalId, NECTERE_EVENT_MSG_CANAL, "Cannot message canal: User has not joined it yet");
					return;
				}
				users = std::vector<uint16_t>(clientToSendTo.begin(), clientToSendTo.end());
			}

			Event event;
			VersionHelper::SetupEvent(event);
			event.m_ApplicationID = 0;
			event.m_EventCode = NECTERE_EVENT_MSG_CANAL;
			event.m_CanalID = canalId;
			event.m_Error = false;
			event.m_Data = content;
			m_UserManager->SendEvent(users, event);
			Log(LogType::Standard, "Sending ", content, " to ", users);
		}
	}

	void CanalManager::ListCanal(uint16_t userId)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		std::vector<Dp::Node> canals;
		for (const auto &pair : m_CanalDictionary)
		{
			Dp::Node canalNode;
			canalNode.Add("name", pair.first);
			canalNode.Add("id", static_cast<int>(pair.second));
			canals.emplace_back(canalNode);
		}
		Dp::Node root;
		root.Add("canals", canals);
		SendAnswer(userId, 0, NECTERE_EVENT_LIST_CANAL, Dp::Json::Str(root, { 0, 0, false }));
	}
}