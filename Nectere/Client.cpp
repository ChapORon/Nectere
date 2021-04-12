#include "Client.hpp"

#include "Logger.hpp"

namespace Nectere
{
	Client::Client(const std::string &host, int port) { Connect(host, std::to_string(port)); }
	Client::Client(const std::string &host, const std::string &service) { Connect(host, std::to_string(service)); }

	void Client::Connect(const std::string &host, const std::string &service)
	{
		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::ip::tcp::resolver resolver(io_context);
		boost::asio::connect(socket, resolver.resolve(host, service));
		m_Session = std::make_shared<Session>(0, m_IOContext, std::move(socket), this);
	}

	void Client::OnReceive(uint16_t sessionId, const Event &message)
	{
		if (message.m_ApplicationID == 0 && message.m_EventCode == 666)
			Stop();
	}

	void Client::OnWrite(uint16_t sessionId)
	{

	}

	void Client::CloseSession(uint16_t id)
	{
		boost::asio::post(m_IOContext, [this]() {m_IOContext.stop();});
	}

	void Client::AcceptSession()
	{
		m_Acceptor.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					auto session = std::make_shared<Session>(0, m_IOContext, std::move(socket), this);
					Logger::out.Log("New session opened with ID: ", session->GetID());
					m_Sessions.emplace_back(session);
				}
				AcceptSession();
			});
	}

	void Client::Send(const Event &event)
	{
		m_Session->Send(event);
	}

	void Client::Start()
	{
		m_IOContext.run();
	}

	void Client::Stop()
	{
		m_Session->Close();
	}
}