#ifdef USE_BOOST

#pragma once

#include <unordered_set>
#include "Network/AServer.hpp"
#include "Network/BoostNetworkUser.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	namespace Network
	{
		class BoostNetworkServer : public AServer
		{
		private:
			std::atomic_bool m_IsClosing;
			std::atomic_bool m_Closed;
			boost::asio::io_context m_IOContext;
			boost::asio::ip::tcp::acceptor m_Acceptor;
			std::unordered_set<uint16_t> m_Sessions;

		private:
			void CloseSession(uint16_t);
			void AcceptSession();

		public:
			BoostNetworkServer(int, Concurrency::ThreadSystem *, UserManager *);
			bool Start() override;
			void Stop() override;
		};
	}
}

#endif