#include "Server.hpp"

#include "Logger.hpp"

namespace Nectere
{
	Session::Session(uint16_t id, boost::asio::io_context &ioContext, boost::asio::ip::tcp::socket socket, ISessionHandler *handler):
		m_SessionID(id),
		m_Closed(false),
		m_Handler(handler),
		m_IOContext(ioContext),
		m_Socket(std::move(socket))
	{
		ReadHeader();
	}

	void Session::ReadHeader()
	{
		boost::asio::async_read(m_Socket, boost::asio::buffer(m_HeaderData, sizeof(Session::Header)), [this](boost::system::error_code ec, size_t) {
			if (!ec)
			{
				LOG(LogType::Standard, '[', m_SessionID, "] Decoding header");
				std::memcpy(&m_Header, m_HeaderData, sizeof(Header));
				if (m_MessageData)
					delete[](m_MessageData);
				m_MessageData = new char[m_Header.messageLength + 1];
				LOG(LogType::Standard, '[', m_SessionID, "] Header decoded, reading ", m_Header.messageLength, " bytes");
				ReadData();
			}
			else if (!m_Closed.load())
				Close();
		});
	}

	void Session::ReadData()
	{
		boost::asio::async_read(m_Socket, boost::asio::buffer(m_MessageData, m_Header.messageLength), [this](boost::system::error_code ec, size_t length) {
			if (!ec)
			{
				if (m_MessageData)
				{
					m_MessageData[length] = '\0';
					m_Message = std::string(m_MessageData);
					delete[](m_MessageData);
					m_MessageData = nullptr;
				}
				Event event = Event{ m_Header.applicationID, m_Header.messageType, m_Message };
				LOG(LogType::Standard, '[', m_SessionID, "] Received: \"", event.m_Data, '"');
				m_Handler->OnReceive(m_SessionID, event);
				ReadHeader();
			}
			else if (!m_Closed.load())
				Close();
		});
	}

	void Session::Send(const Event &event)
	{
		boost::asio::post(m_IOContext, [this, event]() {
			Header header;
			header.applicationID = event.m_ApplicationID;
			header.messageType = event.m_EventCode;
			header.messageLength = event.m_Data.length();
			header.apiVersion = 0;
			size_t length = sizeof(Header) + header.messageLength;
			char *data = new char[length]();
			std::memcpy(data, &header, sizeof(Header));
			std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
			boost::asio::async_write(m_Socket, boost::asio::buffer(data, length), [this, data](boost::system::error_code ec, size_t) {
				delete[](data);
				if (!ec)
					m_Handler->OnWrite(m_SessionID);
				else if (!m_Closed.load())
					Close();
			});
		});
	}

	void Session::Close()
	{
		if (!m_Closed.load())
		{
			boost::asio::post(m_IOContext, [this]() { m_Socket.close(); });
			m_Handler->CloseSession(m_SessionID);
			m_Closed.store(true);
		}
	}

	Session::~Session()
	{
		if (m_MessageData)
			delete[](m_MessageData);
	}
}