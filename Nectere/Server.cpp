#include "Server.hpp"

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <filesystem>
#include "Command/StopCommand.hpp"
#include "Logger.hpp"

typedef std::string FUNCTION(ApplicationName)();
typedef void FUNCTION(ApplicationLoader)(const std::shared_ptr<Nectere::Application> &);

namespace Nectere
{
	Server::Server(int port) :
		m_IsClosing(false),
		m_Closed(false),
		m_Acceptor(m_IOContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		GenerateNectereApplication();
		AcceptSession();
	}

	void Server::OnReceive(uint16_t sessionId, const Event &message)
	{
		if (const std::shared_ptr<Application> &application = m_Applications.Get(message.m_ApplicationID))
		{
			if (application->IsEventAllowed(message))
				application->Treat(sessionId, message);
		}
	}

	void Server::OnWrite(uint16_t sessionId)
	{

	}

	void Server::CloseSession(uint16_t id)
	{
		Logger::out.Log("Closing session with ID: ", id);
		m_SessionIDGenerator.FreeID(id);
		if (!m_IsClosing.load())
			m_Sessions.Remove(id);
	}

	void Server::AcceptSession()
	{
		m_Acceptor.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					auto session = std::make_shared<Session>(m_SessionIDGenerator.GenerateID(), m_IOContext, std::move(socket), this);
					Logger::out.Log("New session opened with ID: ", session->GetID());
					m_Sessions.Add(session);
				}
				AcceptSession();
			});
	}

	void Server::GenerateNectereApplication()
	{
		std::shared_ptr<Application> application = std::make_shared<Application>(m_ApplicationIDGenerator.GenerateID(), "Nectere");
		application->AddCommand(std::make_shared<Command::StopCommand>(this));
		m_Applications.Add(application);
	}

	void Server::LoadApplication(const std::filesystem::path &path)
	{
		std::string moduleName = path.filename().string();
		DynamicLibrary dynamicLibrary(std::filesystem::absolute(path).string());
		if (!dynamicLibrary)
			return;
		Logger::out.Log(moduleName, ": Module loaded");
		ApplicationName applicationName = dynamicLibrary.Load<ApplicationName>("Nectere_ApplicationName");
		if (!applicationName)
			return;
		std::string name = applicationName();
		if (name.empty())
			return;
		Logger::out.Log(moduleName, ": Application name is ", name);
		ApplicationLoader applicationLoader = dynamicLibrary.Load<ApplicationLoader>("Nectere_ApplicationLoader");
		if (!applicationLoader)
			return;
		Logger::out.Log(moduleName, ": Loading application");
		std::shared_ptr<Application> application = std::make_shared<Application>(m_ApplicationIDGenerator.GenerateID(), name);
		applicationLoader(application);
		m_LoadedLibrary.emplace_back(dynamicLibrary);
		m_Applications.Add(application);
		Logger::out.Log(moduleName, ": Application loaded");
	}

	void Server::Update()
	{
		if (!m_Closed.load() && !m_IsClosing.load())
		{
			for (const auto &application : m_Applications.GetElements())
				application->Update();
			boost::asio::post(m_IOContext, [this]() { Update(); });
		}
	}

	void Server::LoadApplications()
	{
		try
		{
			for (const auto &entry : std::filesystem::directory_iterator("plugins"))
			{
				if (!entry.is_directory())
					LoadApplication(entry.path());
			}
		}
		catch (const std::exception &) {}
	}

	void Server::Send(uint16_t sessionID, const Event &event)
	{
		if (const std::shared_ptr<Session> &session = m_Sessions.Get(sessionID))
			session->Send(event);
	}

	void Server::Start()
	{
		boost::asio::post(m_IOContext, [this]() { Update(); });
		m_IOContext.run();
	}

	void Server::Stop()
	{
		if (!m_Closed.load())
		{
			boost::asio::post(m_IOContext, [this]() {
				Logger::out.Log("Closing server");
				m_IsClosing.store(true);
				for (const auto &session : m_Sessions.GetElements())
					session->Close();
				m_IOContext.stop();
				m_Closed.store(true);
			});
		}
	}
}
