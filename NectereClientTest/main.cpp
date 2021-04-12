#include <boost/asio.hpp>
#include <iostream>
#include "Event.hpp"

struct Header
{
	uint16_t applicationID;
	uint16_t messageType;
	uint16_t apiVersion;
	uint64_t messageLength;
};

static void Send(const Nectere::Event &event, boost::asio::ip::tcp::socket &s)
{
	Header header;
	header.applicationID = event.m_ApplicationID;
	header.messageType = event.m_EventCode;
	header.messageLength = event.m_Data.length();
	header.apiVersion = 0;
	size_t length = sizeof(Header) + header.messageLength;
	char *data = new char[length]();
	std::memcpy(data, &header, sizeof(Header));
	std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
	boost::asio::write(s, boost::asio::buffer(data, length));
	delete[](data);
}

int main()
{
	try
	{
		boost::asio::io_context io_context;
		boost::asio::ip::tcp::socket s(io_context);
		boost::asio::ip::tcp::resolver resolver(io_context);
		boost::asio::connect(s, resolver.resolve("127.0.0.1", "4242"));
		Nectere::Event event{ 0, 666, "" };
		Send(event, s);
	}
	catch (std::exception e)
	{
		std::cerr << "Cannot connect to localhost: " << e.what() << std::endl;
	}
	return 0;
}
