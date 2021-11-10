#include "NecterePrompt.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

struct Event
{
	uint16_t m_ApplicationID;
	uint16_t m_EventCode;
	std::string m_Data;
};

struct Header
{
	uint16_t applicationID;
	uint16_t messageType;
	uint16_t apiVersion;
	uint64_t messageLength;
};

std::string NecterePrompt::Send(uint16_t applicationID, uint16_t commandID, const std::string &eventData, bool await)
{
	if (m_Connected)
	{
		Event event{ applicationID, commandID, eventData };
		Header header;
		header.applicationID = event.m_ApplicationID;
		header.messageType = event.m_EventCode;
		header.messageLength = event.m_Data.length();
		header.apiVersion = 0;
		size_t length = sizeof(Header) + header.messageLength;
		char *data = new char[length]();
		std::memcpy(data, &header, sizeof(Header));
		std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
		boost::asio::write(m_Socket, boost::asio::buffer(data, length));
		delete[](data);

		if (await)
		{
			data = new char[sizeof(Header)]();
			boost::asio::read(m_Socket, boost::asio::buffer(data, sizeof(Header)));
			std::memcpy(&header, data, sizeof(Header));
			delete[](data);
			data = new char[header.messageLength + 1];
			length = boost::asio::read(m_Socket, boost::asio::buffer(data, header.messageLength));
			data[length] = '\0';
			std::string msg(data);
			delete[](data);
			return msg;
		}
	}
	return "";
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
}

void NecterePrompt::HandleConnect(const std::vector<std::string> &args)
{
	if (args.size() == 2)
	{
		try
		{
			boost::asio::connect(m_Socket, m_Resolver.resolve(args[0], args[1]));
			m_Connected = true;
			m_IP = args[0];
			m_Port = args[1];
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
	Send(0, 666, "", true);
	OnStop();
}

void NecterePrompt::HandleDisconnect()
{
	OnStop();
}

void NecterePrompt::HandleList()
{
	std::string answer = Send(0, 600, "", true);
	std::stringstream ss{ answer };
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(ss, pt);
	for (auto &item : pt.get_child("applications"))
	{
		auto &applicationNameNode = item.second.get_child("name");
		auto &applicationIDNode = item.second.get_child("id");
		std::cout << "- " << applicationNameNode.get_value<std::string>() << " (" << applicationIDNode.get_value<int>() << ')' << std::endl;
	}
}

void NecterePrompt::HandleQuery(const std::vector<std::string> &args)
{
	if (args.size() == 1)
	{
		std::string answer = Send(0, 601, args[0], true);
		std::stringstream ss{ answer };
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);
		for (auto &item : pt.get_child("commands"))
		{
			auto &commandNameNode = item.second.get_child("name");
			auto &commandIDNode = item.second.get_child("id");
			std::cout << "- " << commandNameNode.get_value<std::string>() << " (" << commandIDNode.get_value<int>() << ')' << std::endl;
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
					std::cout << Send(applicationID, eventCode, args[2], true) << std::endl;
				else
					std::cout << Send(applicationID, eventCode, "", true) << std::endl;
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