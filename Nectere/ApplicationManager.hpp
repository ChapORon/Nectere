#pragma once

#include <filesystem>
#include <unordered_map>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
#include "Concurrency/TaskResult.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	class ThreadSystem;
	class UserManager;
	namespace Network
	{
		class ApplicationEventReceiver;
		class AServer;
	}
	class ApplicationManager final
	{
		friend class Network::ApplicationEventReceiver;
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

	public:
		ApplicationManager(UserManager *);
		Application *CreateNewApplication(const std::string &);
        void Update();
		void LoadApplications();
		~ApplicationManager();
	};
}