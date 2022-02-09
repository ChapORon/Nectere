#ifdef USE_BOOST

#pragma once

#include <boost/asio.hpp>
#include "AUser.hpp"
#include "Logger.hpp"
#include "Network/Header.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	namespace Network
	{
		class BoostNetworkUser: public AUser
		{
			friend class BoostNetworkServer;
		private:
			const Logger *m_Logger = nullptr;
			std::atomic_bool m_Closed;
			boost::asio::io_context &m_IOContext;
			boost::asio::ip::tcp::socket m_Socket;

		private:
			void Read();
			void Receive(const Event &) override;

			template <class... t_Args>
			void Log(LogType logType, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->Log(logType, std::forward<t_Args>(args)...);
			}

			template <class... t_Args>
			void DebugLog(const std::string &function, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->DebugLog("BoostNetworkUser", function, std::forward<t_Args>(args)...);
			}

		public:
			BoostNetworkUser(const Logger *, boost::asio::io_context &, boost::asio::ip::tcp::socket);
			void OnClose() override;
			~BoostNetworkUser();
		};
	}
}

#endif