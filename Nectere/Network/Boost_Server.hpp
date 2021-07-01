#ifdef USE_BOOST

#pragma once

#include "Network/AServer.hpp"
#include "Network/Boost_Session.hpp"
#include "UIDVector.hpp"

namespace Nectere
{
	namespace Network
	{
		class Boost_Server : public AServer
		{
		private:
			std::atomic_bool m_IsClosing;
			std::atomic_bool m_Closed;
			boost::asio::io_context m_IOContext;
			boost::asio::ip::tcp::acceptor m_Acceptor;
			UIDVector<Boost_Session> m_Sessions;

		private:
			void CloseSession(uint16_t);
			void AcceptSession();

		public:
			Boost_Server(int, ThreadSystem *, IEventReceiver *);
			void Send(uint16_t, const Event &) override;
			bool Start() override;
			void Stop() override;
		};
	}
}

#endif