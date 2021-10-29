#pragma once

#include <boost/asio.hpp>
#include "Prompt.hpp"

class NecterePrompt : public Prompt
{
private:
	bool m_Connected{ false };
	std::string m_IP;
	std::string m_Port;
	boost::asio::io_context m_IOContext;
	boost::asio::ip::tcp::socket m_Socket;
	boost::asio::ip::tcp::resolver m_Resolver;

private:
	std::string Send(uint16_t, uint16_t, const std::string &, bool);
	void RecomputePrompt() override;
	void OnStop() override;

	void HandleConnect(const std::vector<std::string> &);
	void HandleStop();
	void HandleDisconnect();
	void HandleQuit(const std::vector<std::string> &);
	void HandleList();
	void HandleQuery(const std::vector<std::string> &);
	void HandleEvent(const std::vector<std::string> &);

public:
	NecterePrompt();
};