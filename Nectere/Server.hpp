#pragma once

#include <filesystem>
#include "Application.hpp"
#include "DynamicLibrary.hpp"
#include "IDGenerator.hpp"
#include "Session.hpp"
#include "UIDVector.hpp"

namespace Nectere
{
	class Server: public ISessionHandler
	{
	private:
		IDGenerator m_SessionIDGenerator;
		IDGenerator m_ApplicationIDGenerator;
		std::atomic_bool m_IsClosing;
		std::atomic_bool m_Closed;
		boost::asio::io_context m_IOContext;
		boost::asio::ip::tcp::acceptor m_Acceptor;
		UIDVector<Session> m_Sessions;
		UIDVector<Application> m_Applications;
		UIDVector<ACommand> m_Commands;
		std::unordered_map<std::string, std::pair<std::shared_ptr<DynamicLibrary>, std::shared_ptr<Application>>> m_LoadedLibrary;

	private:
		void OnReceive(uint16_t, const Event &) override;
		void OnWrite(uint16_t) override;
		void CloseSession(uint16_t) override;
		void AcceptSession();
		void GenerateNectereApplication();
		void LoadApplication(const std::filesystem::path &);
		void Update();

	public:
		Server();
		void LoadApplications();
		void Send(uint16_t, const Event &);
		void Start();
		void Stop();
	};
}