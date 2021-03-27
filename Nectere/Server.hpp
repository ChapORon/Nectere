#pragma once

#include "Application.hpp"
#include "IDGenerator.hpp"
#include "Session.hpp"
#include "UIDVector.hpp"

namespace Nectere
{
	class Server: public ISessionHandler
	{
	private:
		IDGenerator m_SessionIDGenerator;
		IDGenerator m_ApplicationIDGenerator;
		std::atomic_bool m_IsClosing;
		boost::asio::io_context m_IOContext;
		boost::asio::ip::tcp::acceptor m_Acceptor;
		UIDVector<Session> m_Sessions;
		UIDVector<Application> m_Applications;
		UIDVector<ACommand> m_Commands;

	private:
		void OnReceive(uint16_t, const Event &) override;
		void OnWrite(uint16_t) override;
		void CloseSession(uint16_t) override;
		void TreatEvent(const Event &);
		void AcceptSession();

	public:
		Server(int);
		void Start();
		void Stop();
	};
}