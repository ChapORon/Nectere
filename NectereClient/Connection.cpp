#include "Connection.hpp"

#include <iostream>
#include "Defines.hpp"

namespace NectereClient
{
	Connection::Connection() : m_IOContext(), m_Socket(m_IOContext), m_Resolver(m_IOContext) {}

	std::string Connection::Send(uint16_t commandID, const std::string &eventData)
	{
		if (m_Connected)
		{
			Event event{ 0, commandID, eventData };
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
		return "";
	}

	void Connection::Connect(const std::string &ip, const std::string &port)
	{
		try
		{
			boost::asio::connect(m_Socket, m_Resolver.resolve(ip, port));
			m_Connected = true;
		}
		catch (std::exception e)
		{
			std::cerr << "Cannot connect to " << ip << ':' << port << ": " << e.what() << std::endl;
		}
	}

	void Connection::StopServer()
	{
		Send(666, "");
		Disconnect();
	}

	void Connection::Disconnect()
	{
		m_Socket.close();
		m_Connected = false;
	}

	void Connection::TreatLine(const std::vector<std::string> &args)
	{
		if (args[0] == "stop")
			StopServer();
		else if (args[0] == "connect")
			Connect(args[1], args[2]);
		else if (args[0] == "disconnect")
			Disconnect();
		else if (args[0] == "quit")
		{
			Disconnect();
			m_Running = false;
		}
	}
}