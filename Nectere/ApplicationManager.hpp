#pragma once

#include <unordered_map>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
#include "Logger.hpp"
#include "Ptr.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	class ThreadSystem;
	class UserManager;

	class IApplicationManager
	{
	public:
		virtual const std::vector<Ptr<Application>> &GetApplications() const = 0;
		virtual bool HaveApplication(uint16_t) const = 0;
		virtual bool LoadApplication(const std::string &) = 0;
		virtual bool UnloadApplication(uint16_t) = 0;
		virtual bool ReloadApplication(uint16_t) = 0;
	};

	class ApplicationManager final : public IApplicationManager
	{
		friend class Application;
		friend class Manager;
		friend class UserManager;
	private:
		Ptr<UserManager> m_UserManager;
		IDGenerator m_ApplicationIDGenerator;
		const Logger *m_Logger = nullptr;
		UIDSet<Application> m_Applications;
		std::unordered_map<uint16_t, std::pair<DynamicLibrary *, Application *>> m_LoadedApplication;
		std::unordered_map<std::string, uint16_t> m_LoadedLibrary;

	private:
		void Receive(uint16_t, const Event &);
		void SendEvent(uint16_t, const Event &);
		void SendEvent(const std::vector<uint16_t> &, const Event &);
		Ptr<Application> CreateBaseApplication();
		void SetLogger(const Logger *logger) { m_Logger = logger; }

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
				m_Logger->DebugLog("ApplicationManager", function, std::forward<t_Args>(args)...);
		}

	public:
		NECTERE_EXPORT ApplicationManager(const Ptr<UserManager> &);
		NECTERE_EXPORT Ptr<Application> CreateNewApplication(const std::string &);
		inline const std::vector<Ptr<Application>> &GetApplications() const override { return m_Applications.GetElements(); }
		NECTERE_EXPORT void Update();
		inline bool HaveApplication(uint16_t applicationID) const override { return m_Applications.Find(applicationID); }
		NECTERE_EXPORT bool LoadApplication(const std::string &) override;
		NECTERE_EXPORT bool UnloadApplication(uint16_t) override;
		NECTERE_EXPORT bool ReloadApplication(uint16_t) override;
		NECTERE_EXPORT bool IsAuthenticated(uint16_t userId);
		NECTERE_EXPORT ~ApplicationManager();
	};
}