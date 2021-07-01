#ifdef USE_BOOST

#pragma once

#include <boost/asio.hpp>
#include "Network/Header.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	namespace Network
	{
		class Boost_Session
		{
		private:
			char m_HeaderData[sizeof(Header)];
			char *m_MessageData{ nullptr };

		private:
			uint16_t m_SessionID;
			std::atomic_bool m_Closed;
			IEventReceiver *m_Handler;
			Header m_Header;
			std::string m_Message;
			boost::asio::io_context &m_IOContext;
			boost::asio::ip::tcp::socket m_Socket;

		private:
			void ReadHeader();
			void ReadData();

		public:
			Boost_Session(uint16_t, boost::asio::io_context &, boost::asio::ip::tcp::socket, IEventReceiver *);
			void Send(const Event &);
			void Close();
			uint16_t GetID() const { return m_SessionID; }
			~Boost_Session();
		};
	}
}

#endif