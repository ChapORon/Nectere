#pragma once

#include <mutex>
#include <unordered_map>
#include "nectere_export.h"
#include "Canal.hpp"
#include "Configuration.hpp"
#include "IDGenerator.hpp"
#include "Logger.hpp"

namespace Nectere
{
	class Manager;
	class UserManager;
	class CanalManager
	{
		friend class Manager;
		friend class UserManager;
	private:
		struct ConnectedClient
		{
			bool m_IsAuth{ false };
			std::unordered_set<uint16_t> m_Links;
		};

	private:
		std::mutex m_Mutex;
		IDGenerator m_IDGenerator;
		const Logger *m_Logger = nullptr;
		std::unordered_map<std::string, uint16_t> m_CanalDictionary;
		std::unordered_map<uint16_t, Canal> m_Canals;
		std::unordered_map<uint16_t, ConnectedClient> m_ClientLinks;
		UserManager *m_UserManager;

	private:
		void SendAnswer(uint16_t, uint16_t, uint16_t, const std::string &);
		void SendError(uint16_t, uint16_t, uint16_t, const std::string &);
		Canal &GetCanal(uint16_t);
		Canal &GetCanal(const std::string &);
		bool HaveCanal(uint16_t);
		bool HaveCanal(const std::string &);
		bool CreateCanal(const std::string &, const std::string &, int, uint16_t &);
		void DeleteCanal(uint16_t);
		void SetLogger(const Logger *logger) { m_Logger = logger; }
		void InitConfiguration(const Configuration &);

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
				m_Logger->DebugLog("CanalManager", function, std::forward<t_Args>(args)...);
		}

	public:
		CanalManager(UserManager *);
		void Update();
		NECTERE_EXPORT void CreateCanal(uint16_t, const std::string &);
		NECTERE_EXPORT void DeleteCanal(uint16_t, uint16_t);
		NECTERE_EXPORT void JoinCanal(uint16_t, uint16_t, const std::string &);
		NECTERE_EXPORT void LeaveCanal(uint16_t, uint16_t);
		NECTERE_EXPORT void MessageCanal(uint16_t, uint16_t, const std::string &);
		NECTERE_EXPORT void LeaveAllCanal(uint16_t);
		NECTERE_EXPORT void ListCanal(uint16_t);
	};
}