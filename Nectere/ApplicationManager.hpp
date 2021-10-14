#pragma once

#include <filesystem>
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
	class ApplicationManager final
	{
		friend class Application;
		friend class UserManager;
	private:
		UserManager *m_UserManager;
		IDGenerator m_ApplicationIDGenerator;
		UIDSet<Application> m_Applications;
		UIDSet<ACommand> m_Commands;
		std::unordered_map<std::string, std::pair<DynamicLibrary *, Application *>> m_LoadedLibrary;

	private:
		void LoadApplication(const std::filesystem::path &);
		void Receive(uint16_t, const Event &);
		void SendEvent(uint16_t, const Event &);
		void SendEvent(const std::vector<uint16_t> &, const Event &);

	public:
		ApplicationManager(UserManager *);
		Ptr<Application> CreateNewApplication(const std::string &);
        void Update();
		void LoadApplications();
		~ApplicationManager();
	};
}