#ifdef USE_BOOST

#pragma once

#include <boost/asio.hpp>
#include "AUser.hpp"
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
			struct BoostSocket
			{
				boost::asio::io_context &m_IOContext;
				boost::asio::ip::tcp::socket m_Socket;
			};

		private:
			char m_HeaderData[sizeof(Header)];
			char *m_MessageData{ nullptr };

		private:
			std::atomic_bool m_Closed;
			Header m_Header;
			std::string m_Message;
			BoostSocket *m_BoostSocket;

		private:
			void ReadHeader();
			void ReadData();
			void Receive(const Event &) override;
			void InternalInit(boost::asio::io_context &, boost::asio::ip::tcp::socket);

		public:
			BoostNetworkUser(BoostSocket *);
			void Close();
			~BoostNetworkUser();
		};
	}
}

#endif