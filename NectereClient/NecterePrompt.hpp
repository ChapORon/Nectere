#pragma once

#include <boost/asio.hpp>
#include "Prompt.hpp"

struct Version
{
	uint16_t m_Major;
	uint16_t m_Minor;
	uint16_t m_Patch;
};

struct Event
{
	bool m_Error;
	uint16_t m_CanalID;
	uint16_t m_ApplicationID;
	uint16_t m_UserID;
	uint16_t m_EventCode;
	Version m_APIVersion;
	std::string m_Data;
};

struct Header
{
	uint8_t error;
	uint16_t canalID;
	uint16_t applicationID;
	uint16_t userID;
	uint16_t messageType;
	uint16_t apiVersion;
	uint64_t messageLength;
};

class NecterePrompt : public Prompt
{
private:
	bool m_Connected{ false };
	uint16_t m_ID;
	std::string m_IP;
	std::string m_Port;
	std::unordered_map<std::string, uint16_t> m_Canals;
	boost::asio::io_context m_IOContext;
	boost::asio::ip::tcp::socket m_Socket;
	boost::asio::ip::tcp::resolver m_Resolver;

private:
	Event Receive();
	void Send(uint16_t, uint16_t, uint16_t, const std::string &);
	void RecomputePrompt() override;
	void OnStop() override;

	void HandleConnect(const std::vector<std::string> &);
	void HandleStop();
	void HandleDisconnect();
	void HandleQuit(const std::vector<std::string> &);
	void HandleList();
	void HandleQuery(const std::vector<std::string> &);
	void HandleEvent(const std::vector<std::string> &);
	void HandleAwait();
	void HandlePing(const std::vector<std::string> &);
	void HandleAuth(const std::vector<std::string> &);

	//Canal
	void UpdateCanalList();
	bool GetCanalId(const std::string &, uint16_t &) const;
	void HandleCanalMsg(const std::vector<std::string> &);
	void HandleCanalCreate(const std::vector<std::string> &);
	void HandleCanalDelete(const std::vector<std::string> &);
	void HandleCanalJoin(const std::vector<std::string> &);
	void HandleCanalLeave(const std::vector<std::string> &);
	void HandleCanalList();

public:
	NecterePrompt();
};