#include "NecterePrompt.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include "../Nectere/NectereEventCode.hpp"

Event NecterePrompt::Receive()
{
	Header header;
	char *data = new char[sizeof(Header)]();
	size_t length = boost::asio::read(m_Socket, boost::asio::buffer(data, sizeof(Header)));
	if (length == sizeof(Header))
	{
		std::memcpy(&header, data, sizeof(Header));
		delete[](data);
		data = new char[header.messageLength + 1];
		length = boost::asio::read(m_Socket, boost::asio::buffer(data, header.messageLength));
		if (length == header.messageLength)
		{
			data[length] = '\0';
			std::string msg(data);
			delete[](data);

			Event event;
			event.m_Error = static_cast<bool>(header.error);
			event.m_CanalID = header.canalID;
			event.m_ApplicationID = header.applicationID;
			event.m_UserID = header.userID;
			event.m_EventCode = header.messageType;
			uint16_t version = header.apiVersion;
			event.m_APIVersion.m_Patch = version % 100;
			version /= 100;
			event.m_APIVersion.m_Minor = version % 100;
			version /= 100;
			event.m_APIVersion.m_Major = version % 100;
			event.m_Data = msg;
			return event;
		}
	}
	OnStop();
	Event event;
	event.m_Error = true;
	return event;
}

void NecterePrompt::Send(uint16_t applicationID, uint16_t commandID, uint16_t canalID, const std::string &eventData)
{
	if (m_Connected)
	{
		Event event;
		event.m_Error = false;
		event.m_CanalID = canalID;
		event.m_ApplicationID = applicationID;
		event.m_UserID = m_ID;
		event.m_EventCode = commandID;
		event.m_Data = eventData;
		event.m_APIVersion.m_Major = 0;
		event.m_APIVersion.m_Minor = 0;
		event.m_APIVersion.m_Patch = 1;

		Header header;
		header.error = static_cast<uint8_t>(event.m_Error);
		header.canalID = event.m_CanalID;
		header.userID = event.m_UserID;
		header.applicationID = event.m_ApplicationID;
		header.messageType = event.m_EventCode;
		header.apiVersion = (((event.m_APIVersion.m_Major * 100) + event.m_APIVersion.m_Minor) * 100) + event.m_APIVersion.m_Patch;
		header.messageLength = event.m_Data.length();
		size_t length = sizeof(Header) + header.messageLength;
		char *data = new char[length]();
		std::memcpy(data, &header, sizeof(Header));
		std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
		boost::asio::write(m_Socket, boost::asio::buffer(data, length));
		delete[](data);
	}
}

void NecterePrompt::RecomputePrompt()
{
	if (m_Connected)
	{
		std::stringstream buffer;
		buffer << '[' << m_IP << ':' << m_Port << "]> ";
		SetPrompt(buffer.str());
	}
	else
		SetPrompt("[]> ");
}

void NecterePrompt::OnStop()
{
	m_Socket.close();
	m_Connected = false;
}

NecterePrompt::NecterePrompt(): Prompt("[]> "), m_IOContext(), m_Socket(m_IOContext), m_Resolver(m_IOContext)
{
	AddCommand("connect", this, &NecterePrompt::HandleConnect);
	AddCommand("disconnect", this, &NecterePrompt::HandleDisconnect);
	AddCommand("stop", this, &NecterePrompt::HandleStop);
	AddCommand("list", this, &NecterePrompt::HandleList);
	AddCommand("query", this, &NecterePrompt::HandleQuery);
	AddCommand("event", this, &NecterePrompt::HandleEvent);
	AddCommand("await", this, &NecterePrompt::HandleAwait);
	AddCommand("ping", this, &NecterePrompt::HandlePing);
	AddCommand("auth", this, &NecterePrompt::HandleAuth);

	CommandTreater canalCommandTreater;
	canalCommandTreater.AddCommand("msg", this, &NecterePrompt::HandleCanalMsg);
	canalCommandTreater.AddCommand("create", this, &NecterePrompt::HandleCanalCreate);
	canalCommandTreater.AddCommand("delete", this, &NecterePrompt::HandleCanalDelete);
	canalCommandTreater.AddCommand("join", this, &NecterePrompt::HandleCanalJoin);
	canalCommandTreater.AddCommand("leave", this, &NecterePrompt::HandleCanalLeave);
	canalCommandTreater.AddCommand("list", this, &NecterePrompt::HandleCanalList);
	AddSubCommand("canal", canalCommandTreater);
}

void NecterePrompt::HandleConnect(const std::vector<std::string> &args)
{
	if (args.size() == 2)
	{
		try
		{
			boost::asio::connect(m_Socket, m_Resolver.resolve(args[0], args[1]));
			Event event = Receive();
			m_ID = event.m_UserID;
			m_Connected = true;
			m_IP = args[0];
			m_Port = args[1];
			std::cout << "Connected to " << args[0] << ':' << args[1] << ": Your ID is " << m_ID << std::endl;
		}
		catch (std::exception e)
		{
			std::cerr << "Cannot connect to " << args[0] << ':' << args[1] << ": " << e.what() << std::endl;
		}
	}
	else if (args.size() < 2)
		std::cerr << "Error: Not enough argument: connect [ip] [port]" << std::endl;
	else
		std::cerr << "Error: Too much argument: connect [ip] [port]" << std::endl;
}

void NecterePrompt::HandleStop()
{
	Send(0, NECTERE_EVENT_STOP, 0, "");
	OnStop();
}

void NecterePrompt::HandleDisconnect()
{
	OnStop();
}

void NecterePrompt::HandleList()
{
	Send(0, NECTERE_EVENT_APP_LIST, 0, "");
	Event event = Receive();
	if (event.m_Error)
		std::cerr << event.m_Data << std::endl;
	else
	{
		std::stringstream ss{ event.m_Data };
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);
		for (auto &item : pt.get_child("applications"))
		{
			auto &applicationNameNode = item.second.get_child("name");
			auto &applicationIDNode = item.second.get_child("id");
			std::cout << "- " << applicationNameNode.get_value<std::string>() << " (" << applicationIDNode.get_value<int>() << ')' << std::endl;
		}
	}
}

void NecterePrompt::HandleQuery(const std::vector<std::string> &args)
{
	if (args.size() == 1)
	{
		Send(0, NECTERE_EVENT_CMD_LIST, 0, args[0]);
		Event event = Receive();
		if (event.m_Error)
			std::cerr << event.m_Data << std::endl;
		else
		{
			std::stringstream ss{ event.m_Data };
			boost::property_tree::ptree pt;
			boost::property_tree::read_json(ss, pt);
			for (auto &item : pt.get_child("commands"))
			{
				auto &commandNameNode = item.second.get_child("name");
				auto &commandIDNode = item.second.get_child("id");
				std::cout << "- " << commandNameNode.get_value<std::string>() << " (" << commandIDNode.get_value<int>() << ')' << std::endl;
			}
		}
	}
	else if (args.size() < 1)
		std::cerr << "Error: Not enough argument: query [application]" << std::endl;
	else
		std::cerr << "Error: Too much argument: query [application]" << std::endl;
}

void NecterePrompt::HandleEvent(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size == 2 || size == 3)
	{
		try
		{
			int applicationID = std::stoi(args[0]);
			try
			{
				int eventCode = std::stoi(args[1]);
				if (size == 3)
					Send(applicationID, eventCode, 0, args[2]);
				else
					Send(applicationID, eventCode, 0, "");
				std::cout << Receive().m_Data << std::endl;
			}
			catch (std::exception e)
			{
				std::cerr << "Cannot send event to application" << applicationID << ": Misformated event code" << std::endl;
			}
		}
		catch (std::exception e)
		{
			std::cerr << "Cannot send event to application: Misformated application id" << std::endl;
		}
	}
	else if (args.size() < 2)
		std::cerr << "Error: Not enough argument: event [application] [code] [optional:data]" << std::endl;
	else
		std::cerr << "Error: Too much argument: event [application] [code] [optional:data]" << std::endl;
}

void NecterePrompt::HandleAwait()
{
	Event event = Receive();
	std::cout << "[application:" << event.m_ApplicationID << ", event:" << event.m_EventCode << "] " << event.m_Data << std::endl;
}

void NecterePrompt::UpdateCanalList()
{
	Send(0, NECTERE_EVENT_LIST_CANAL, 0, "");
	Event event = Receive();
	if (event.m_Error)
		std::cerr << event.m_Data << std::endl;
	else
	{
		std::stringstream ss{ event.m_Data };
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);
		m_Canals.clear();
		for (auto &item : pt.get_child("canals"))
		{
			auto &canalNameNode = item.second.get_child("name");
			auto &canalIDNode = item.second.get_child("id");
			m_Canals[canalNameNode.get_value<std::string>()] = canalIDNode.get_value<int>();
		}
	}
}

bool NecterePrompt::GetCanalId(const std::string &canalName, uint16_t &id) const
{
	auto canalIt = m_Canals.find(canalName);
	if (canalIt != m_Canals.end())
	{
		id = (*canalIt).second;
		return true;
	}
	return false;
}

void NecterePrompt::HandleCanalMsg(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size == 2)
	{
		UpdateCanalList();
		uint16_t canalId;
		if (GetCanalId(args[0], canalId))
			Send(0, NECTERE_EVENT_MSG_CANAL, canalId, args[1]);
		else
			std::cerr << "Error: Cannot message " << args[0] << ": Canal does not exist" << std::endl;
	}
	else if (size < 2)
		std::cerr << "Error: Not enough argument: canal msg [canal] [message]" << std::endl;
	else
		std::cerr << "Error: Too much argument: canal msg [canal] [message]" << std::endl;
}

void NecterePrompt::HandleAuth(const std::vector<std::string> &args)
{
	std::string authKey;
	size_t size = args.size();
	if (size == 1)
		authKey = args[0];
	else if (size > 1)
	{
		std::cerr << "Error: Too much argument: canal auth [optional:auth_key]" << std::endl;
		return;
	}
	Send(0, NECTERE_EVENT_AUTH, 0, authKey);
	std::cout << Receive().m_Data << std::endl;
}

void NecterePrompt::HandleCanalJoin(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size == 1 || size == 2)
	{
		UpdateCanalList();
		uint16_t canalId;
		if (GetCanalId(args[0], canalId))
		{
			if (size == 1)
				Send(0, NECTERE_EVENT_JOIN_CANAL, canalId, "");
			else
				Send(0, NECTERE_EVENT_JOIN_CANAL, canalId, args[1]);
		}
		std::cout << Receive().m_Data << std::endl;
	}
	else if (size < 1)
		std::cerr << "Error: Not enough argument: canal join [canal] [optional:password]" << std::endl;
	else if (size > 2)
		std::cerr << "Error: Too much argument: canal join [canal] [optional:password]" << std::endl;
}

void NecterePrompt::HandleCanalLeave(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size == 1)
	{
		UpdateCanalList();
		uint16_t canalId;
		if (GetCanalId(args[0], canalId))
		{
			Send(0, NECTERE_EVENT_LEAVE_CANAL, canalId, "");
			std::cout << Receive().m_Data << std::endl;
		}
		else
			std::cerr << "Error: Cannot leave " << args[0] << ": Canal does not exist" << std::endl;
	}
	else if (size < 1)
		std::cerr << "Error: Not enough argument: canal leave [canal]" << std::endl;
	else
		std::cerr << "Error: Too much argument: canal leave [canal]" << std::endl;
}

static bool TreatArgument(const std::string &argType, const std::string &value, std::string &out)
{
	if (value.starts_with(argType))
	{
		out = value.substr(argType.length());
		return true;
	}
	return false;
}

void NecterePrompt::HandleCanalCreate(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size >= 1 && size <= 3)
	{
		boost::property_tree::ptree out;
		out.put("name", args[0]);
		for (size_t i = 1; i != size; i++)
		{
			std::string tmp;
			if (TreatArgument("-p=", args[i], tmp))
				out.put("password", tmp);
			else if (TreatArgument("-l=", args[i], tmp))
			{
				try
				{
					out.put("limit", std::stoi(tmp));
				}
				catch (const std::exception &e)
				{
					std::cerr << "Error: Misformated limit \"" << args[i] << "\": canal create [canal] -p=[optional:password] -l=[optional:limit]" << std::endl;
				}
			}
			else
				std::cerr << "Error: Unrecognized argument \"" << args[i] << "\": canal create [canal] -p=[optional:password] -l=[optional:limit]" << std::endl;
		}
		std::ostringstream oss;
		boost::property_tree::write_json(oss, out);
		Send(0, NECTERE_EVENT_CREATE_CANAL, 0, oss.str());
		std::cout << Receive().m_Data << std::endl;
		UpdateCanalList();
	}
	else if (size < 1)
		std::cerr << "Error: Not enough argument: canal create [canal] -p=[optional:password] -l=[optional:limit]" << std::endl;
	else if (size > 3)
		std::cerr << "Error: Too much argument: canal create [canal] -p=[optional:password] -l=[optional:limit]" << std::endl;
}

void NecterePrompt::HandleCanalDelete(const std::vector<std::string> &args)
{
	size_t size = args.size();
	if (size == 1)
	{
		UpdateCanalList();
		uint16_t canalId;
		if (GetCanalId(args[0], canalId))
		{
			Send(0, NECTERE_EVENT_DELETE_CANAL, canalId, "");
			std::cout << Receive().m_Data << std::endl;
		}
		else
			std::cerr << "Error: Cannot delete " << args[0] << ": Canal does not exist" << std::endl;
	}
	else if (size < 1)
		std::cerr << "Error: Not enough argument: canal delete [canal]" << std::endl;
	else
		std::cerr << "Error: Too much argument: canal delete [canal]" << std::endl;
}

void NecterePrompt::HandleCanalList()
{
	for (const auto &pair : m_Canals)
		std::cout << "- " << pair.first << " (" << pair.second << ')' << std::endl;
}

void NecterePrompt::HandlePing(const std::vector<std::string> &args)
{
	std::stringstream stream;
	for (const std::string &arg : args)
		stream << arg;
	std::string pingStr = stream.str();
	Send(0, NECTERE_EVENT_PING, 0, pingStr);
	if (Receive().m_Data == pingStr)
		std::cout << "Pong" << std::endl;
	else
		std::cerr << "Error when pinging the server" << std::endl;
}