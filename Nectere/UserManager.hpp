#pragma once

#include "Event.hpp"
#include "IDGenerator.hpp"
#include "Logger.hpp"
#include "Ptr.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	class AUser;
	class ApplicationManager;
	class CanalManager;
	class UserManager final
	{
		friend class AUser;
		friend class ApplicationManager;
		friend class CanalManager;
		friend class Manager;
	private:
		std::string m_AuthenticationKey;
		ApplicationManager *m_ApplicationManager;
		CanalManager *m_CanalUser;
		const Logger *m_Logger = nullptr;
		UIDSet<AUser> m_Users;
		IDGenerator m_IDGenerator;
		std::vector<uint16_t> m_UsersToRemove;

	private:
		AUser *InternalAddUser(AUser *);
		void ReceiveEvent(uint16_t, const Event &);
		void SendEvent(uint16_t, const Event &);
		void SendEvent(const std::vector<uint16_t> &, const Event &);
		bool IsAuthenticated(uint16_t userId);
		void SetLogger(const Logger *);

		template <class... t_Args>
		void Log(LogType logType, t_Args &&... args) const
		{
			if (m_Logger)
				m_Logger->Log(logType, std::forward<t_Args>(args)...);
		}

		template <class... t_Args>
		void DebugLog(const std::string &function, t_Args &&... args) const
		{
			if (m_Logger)
				m_Logger->DebugLog("UserManager", function, std::forward<t_Args>(args)...);
		}

	public:
		NECTERE_EXPORT UserManager(const std::string &);
		template <typename t_User, typename ...t_Arg>
		Ptr<t_User> AddUser(t_Arg&&... args) { return Ptr(dynamic_cast<t_User *>(InternalAddUser(new t_User(std::forward<t_Arg>(args)...)))); }
		NECTERE_EXPORT void AuthenticateUser(uint16_t, const std::string &);
		NECTERE_EXPORT void RemoveUserInstant(uint16_t);
		NECTERE_EXPORT void RemoveUser(uint16_t);
		inline Ptr<ApplicationManager> GetApplicationManager() { return Ptr(m_ApplicationManager); }
		inline Ptr<CanalManager> GetCanalManager() { return Ptr(m_CanalUser); }
		NECTERE_EXPORT void Update();
		NECTERE_EXPORT ~UserManager();
	};
}