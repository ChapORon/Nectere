#pragma once

#include <filesystem>
#include <unordered_map>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
#include "Network/IEventReceiver.hpp"
#include "TaskResult.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	class ThreadSystem;
	namespace Network
	{
		class AServer;
	}
	class ApplicationManager: public Network::IEventReceiver
	{
	private:
		IDGenerator m_ApplicationIDGenerator;
		UIDSet<Application> m_Applications;
		UIDSet<ACommand> m_Commands;
		std::unordered_map<std::string, std::pair<DynamicLibrary *, Application *>> m_LoadedLibrary;

	private:
		void OnReceive(uint16_t, const Event &) override;
		void GenerateNectereApplication(Network::AServer *, ThreadSystem *);
		void LoadApplication(const std::filesystem::path &);
		TaskResult Update();

	public:
		void LoadApplications(Network::AServer *, ThreadSystem *);
		~ApplicationManager();
	};
}