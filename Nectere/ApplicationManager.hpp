#pragma once

#include <filesystem>
#include <unordered_map>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
#include "Network/IEventReceiver.hpp"
#include "TaskResult.hpp"
#include "UIDVector.hpp"

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
		UIDVector<Application> m_Applications;
		UIDVector<ACommand> m_Commands;
		std::unordered_map<std::string, std::pair<std::shared_ptr<DynamicLibrary>, std::shared_ptr<Application>>> m_LoadedLibrary;

	private:
		void OnReceive(uint16_t, const Event &) override;
		void GenerateNectereApplication(Network::AServer *, ThreadSystem *);
		void LoadApplication(const std::filesystem::path &);
		TaskResult Update();

	public:
		void LoadApplications(Network::AServer *, ThreadSystem *);
	};
}