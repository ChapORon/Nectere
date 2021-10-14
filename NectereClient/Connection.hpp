#pragma once

#include <boost/asio.hpp>

namespace NectereClient
{
	class Connection
	{
	private:
		bool m_Running{ true };
		bool m_Connected{ false };
		boost::asio::io_context m_IOContext;
		boost::asio::ip::tcp::socket m_Socket;
		boost::asio::ip::tcp::resolver m_Resolver;

	private:
		void Connect(const std::string &, const std::string &);
		std::string Send(uint16_t, const std::string &);
		void StopServer();
		void Disconnect();

	public:
		Connection();
		void TreatLine(const std::vector<std::string> &);
		bool IsRunning() { return m_Running; }
	};
}