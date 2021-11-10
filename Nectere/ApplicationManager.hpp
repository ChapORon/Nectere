#pragma once

#include <unordered_map>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
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
		friend class UserManager;
	private:
		UserManager *m_UserManager;
		IDGenerator m_ApplicationIDGenerator;
		UIDSet<Application> m_Applications;
		std::unordered_map<uint16_t, std::pair<DynamicLibrary *, Application *>> m_LoadedApplication;
		std::unordered_map<std::string, uint16_t> m_LoadedLibrary;

	private:
		void Receive(uint16_t, const Event &);
		void SendEvent(uint16_t, const Event &);
		void SendEvent(const std::vector<uint16_t> &, const Event &);

	public:
		ApplicationManager(UserManager *);
		Ptr<Application> CreateNewApplication(const std::string &);
		const std::vector<Ptr<Application>> &GetApplications() const override { return m_Applications.GetElements(); }
		void Update();
		bool HaveApplication(uint16_t applicationID) const override { return m_Applications.Find(applicationID); }
		bool LoadApplication(const std::string &) override;
		bool UnloadApplication(uint16_t) override;
		bool ReloadApplication(uint16_t) override;
		~ApplicationManager();
	};
}