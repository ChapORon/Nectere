#ifdef USE_BOOST

#include "Network/Boost_Server.hpp"
#include "Logger.hpp"
#include "Concurrency/ThreadSystem.hpp"
#include "UserManager.hpp"

namespace Nectere
{
	namespace Network
	{
		Boost_Server::Boost_Server(int port, Concurrency::ThreadSystem *threadSystem, UserManager *userManager) : AServer(port, threadSystem, userManager),
			m_IsClosing(false),
			m_Closed(false),
			m_Acceptor(m_IOContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

		void Boost_Server::CloseSession(uint16_t id)
		{
			LOG(LogType::Standard, "Closing session with ID: ", id);
			m_UserManager->RemoveUser(id);
			m_Sessions.erase(m_Sessions.find(id));
		}

		void Boost_Server::AcceptSession()
		{
			m_Acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!ec)
				{
					uint16_t id = m_UserManager->AddUser<BoostNetworkUser>(new BoostNetworkUser::BoostSocket{ m_IOContext, std::move(socket) });
					LOG(LogType::Standard, "New session opened with ID: ", id);
					m_Sessions.insert(id);
				}
				AcceptSession();
			});
		}

		bool Boost_Server::Start()
		{
			m_ThreadSystem->AddTask([=]() {
				LOG(LogType::Standard, "Server created on port ", m_Port);
				AcceptSession();
				m_IOContext.run();
				LOG(LogType::Standard, "Network stopped");
				return Concurrency::TaskResult::Success;
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
					for (uint16_t sessionUserID : m_Sessions)
						m_UserManager->RemoveUser(sessionUserID);
					m_Sessions.clear();
					LOG(LogType::Standard, "Stopping network");
					m_IOContext.stop();
					m_Closed.store(true);
				});
			}
		}
	}
}

#endif