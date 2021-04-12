#pragma once

#include <boost/asio.hpp>
#include "ISessionHandler.hpp"

namespace Nectere
{
	class Server;

	class Session final
	{
	private:
		struct Header
		{
			uint16_t applicationID;
			uint16_t messageType;
			uint16_t apiVersion;
			uint64_t messageLength;
		};

	private:
		char m_HeaderData[sizeof(Header)];
		char *m_MessageData{ nullptr };

	private:
		uint16_t m_SessionID;
		std::atomic_bool m_Closed;
		ISessionHandler *m_Handler;
		Header m_Header;
		std::string m_Message;
		boost::asio::io_context &m_IOContext;
		boost::asio::ip::tcp::socket m_Socket;

	private:
		void ReadHeader();
		void ReadData();

	public:
		Session(uint16_t, boost::asio::io_context &, boost::asio::ip::tcp::socket, ISessionHandler *);
		void Send(const Event &);
		void Close();
		uint16_t GetID() const { return m_SessionID; }
		~Session();
	};
}