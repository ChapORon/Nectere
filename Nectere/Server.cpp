#include "Server.hpp"

#include <iostream>
#include "Command/StopCommand.hpp"

namespace Nectere
{
	Server::Server(int port) :
		m_IsClosing(false),
		m_Acceptor(m_IOContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		std::shared_ptr<Application> application = std::make_shared<Application>(m_ApplicationIDGenerator.GenerateID(), "Nectere");
		application->AddCommand(std::make_shared<Command::StopCommand>(this));
		m_Applications.Add(application);
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
		std::cout << "Closing session with ID: " << id << std::endl;
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
					std::cout << "New session opened with ID: " << session->GetID() << std::endl;
					m_Sessions.Add(session);
				}
				AcceptSession();
			});
	}

	void Server::Start()
	{
		m_IOContext.run();
	}

	void Server::Stop()
	{
		boost::asio::post(m_IOContext, [this]() {
			std::cout << "Closing server" << std::endl;
			m_IsClosing.store(true);
			for (const auto &session : m_Sessions.GetElements())
				session->Close();
			m_IOContext.stop();
		});
	}
}
