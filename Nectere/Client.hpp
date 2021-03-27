#pragma once

#include "Session.hpp"

namespace Nectere
{
	class Client : public ISessionHandler
	{
	public:
		class Handler
		{
		public:
			virtual void OnReceive(const Event &message) = 0;
			virtual void OnWrite() = 0;
			virtual void OnClose() = 0;
		};
	private:
		boost::asio::io_context m_IOContext;
		std::shared_ptr<Session> m_Session;
		std::shared_ptr<Handler> m_Handler;

	private:
		void OnReceive(uint16_t, const Event &) override;
		void OnWrite(uint16_t) override;
		void CloseSession(uint16_t) override;
		void Connect(const std::string &, const std::string &);

	public:
		Client(const std::string &, int);
		Client(const std::string &, const std::string &);
		void Send(const Event &);
		void Start();
		void Stop();
	};
}