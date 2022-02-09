#ifdef USE_BOOST
#include "Network/BoostNetworkServer.hpp"

#include "Concurrency/ThreadSystem.hpp"
#include "Helper.hpp"
#include "Logger.hpp"
#include "UserManager.hpp"

namespace Nectere
{
	namespace Network
	{
		BoostNetworkServer::BoostNetworkServer(int port, const std::string &whitelistFile, Concurrency::AThreadSystem *threadSystem, const Logger *logger, UserManager *userManager) :
			AServer(port, whitelistFile, threadSystem, logger, userManager),
			m_IsClosing(false),
			m_Closed(false),
			m_Acceptor(m_IOContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

		void BoostNetworkServer::CloseSession(uint16_t id)
		{
			Log(LogType::Standard, "Closing session with ID: ", id);
			m_UserManager->RemoveUser(id);
			m_Sessions.erase(m_Sessions.find(id));
		}

		void BoostNetworkServer::AcceptSession()
		{
			m_Acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!ec)
				{
					std::string clientIP = socket.remote_endpoint().address().to_string();
					Log(LogType::Standard, "New connection on IP ", clientIP);
					if (IsIPWhitelisted(clientIP))
					{
						if (auto networkUser = m_UserManager->AddUser<BoostNetworkUser>(m_Logger, m_IOContext, std::move(socket)))
						{
							uint16_t id = networkUser->GetID();
							Log(LogType::Standard, "New session opened with ID: ", id);
							m_Sessions.insert(id);
						}
					}
				}
				AcceptSession();
			});
		}

		bool BoostNetworkServer::Start()
		{
			m_ThreadSystem->AddTask([=]() {
				Helper::LogHeaderInfo(m_Logger);
				Log(LogType::Standard, "Server created on port ", m_Port);
				AcceptSession();
				m_IOContext.run();
				Log(LogType::Standard, "Network stopped");
				return Concurrency::TaskResult::Success;
			});
			return true;
		}

		void BoostNetworkServer::Stop()
		{
			if (!m_Closed.load())
			{
				boost::asio::post(m_IOContext, [this]() {
					Log(LogType::Standard, "Closing server");
					m_IsClosing.store(true);
					for (uint16_t sessionUserID : m_Sessions)
						m_UserManager->RemoveUserInstant(sessionUserID);
					m_Sessions.clear();
					Log(LogType::Standard, "Stopping network");
					m_IOContext.stop();
					m_Closed.store(true);
				});
			}
		}
	}
}

#endif