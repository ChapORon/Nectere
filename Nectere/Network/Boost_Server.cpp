#ifdef USE_BOOST

#include "Network/Boost_Server.hpp"
#include "Logger.hpp"
#include "ThreadSystem.hpp"

namespace Nectere
{
	namespace Network
	{
		Boost_Server::Boost_Server(int port, ThreadSystem *threadSystem, IEventReceiver *handler) : AServer(port, threadSystem, handler),
			m_IsClosing(false),
			m_Closed(false),
			m_Acceptor(m_IOContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

		void Boost_Server::CloseSession(uint16_t id)
		{
			LOG(LogType::Standard, "Closing session with ID: ", id);
			m_SessionIDGenerator.FreeID(id);
			if (!m_IsClosing.load())
				m_Sessions.Remove(id);
		}

		void Boost_Server::AcceptSession()
		{
			m_Acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!ec)
				{
					auto session = std::make_shared<Boost_Session>(m_SessionIDGenerator.GenerateID(), m_IOContext, std::move(socket), m_EventReceiver);
					LOG(LogType::Standard, "New session opened with ID: ", session->GetID());
					m_Sessions.Add(session);
				}
				AcceptSession();
			});
		}

		void Boost_Server::Send(uint16_t sessionID, const Event &event)
		{
			if (const std::shared_ptr<Boost_Session> &session = m_Sessions.Get(sessionID))
				session->Send(event);
		}

		bool Boost_Server::Start()
		{
			m_ThreadSystem->AddTask([=]() {
				LOG(LogType::Standard, "Server created on port ", m_Port);
				AcceptSession();
				m_IOContext.run();
				LOG(LogType::Standard, "Network stopped");
				return TaskResult::SUCCESS;
			});
			return true;
		}

		void Boost_Server::Stop()
		{
			if (!m_Closed.load())
			{
				boost::asio::post(m_IOContext, [this]() {
					LOG(LogType::Standard, "Closing server");
					m_IsClosing.store(true);
					for (const auto &session : m_Sessions.GetElements())
						session->Close();
					LOG(LogType::Standard, "Stopping network");
					m_IOContext.stop();
					m_Closed.store(true);
				});
			}
		}
	}
}

#endif