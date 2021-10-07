#ifdef USE_BOOST

#include "Network/BoostNetworkUser.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Network
	{
		BoostNetworkUser::BoostNetworkUser(BoostSocket *boostSocket) :
			m_Closed(false),
			m_BoostSocket(boostSocket)
		{
			ReadHeader();
		}

		void BoostNetworkUser::ReadHeader()
		{
			boost::asio::async_read(m_BoostSocket->m_Socket, boost::asio::buffer(m_HeaderData, sizeof(Network::Header)), [this](boost::system::error_code ec, size_t) {
				if (!ec)
				{
					LOG(LogType::Standard, '[', GetID(), "] Decoding header");
					std::memcpy(&m_Header, m_HeaderData, sizeof(Header));
					if (m_MessageData)
						delete[](m_MessageData);
					m_MessageData = new char[m_Header.messageLength + 1];
					LOG(LogType::Standard, '[', GetID(), "] Header decoded, reading ", m_Header.messageLength, " bytes");
					ReadData();
				}
				else if (!m_Closed.load())
					Close();
				});
		}

		void BoostNetworkUser::ReadData()
		{
			boost::asio::async_read(m_BoostSocket->m_Socket, boost::asio::buffer(m_MessageData, m_Header.messageLength), [this](boost::system::error_code ec, size_t length) {
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
					LOG(LogType::Standard, '[', GetID(), "] Received: \"", event.m_Data, '"');
					Send(event);
					if (!m_Closed.load())
						ReadHeader();
				}
				else if (!m_Closed.load())
					Close();
				});
		}

		void BoostNetworkUser::Receive(const Event &event)
		{
			boost::asio::post(m_BoostSocket->m_IOContext, [this, event]() {
				Header header;
				header.applicationID = event.m_ApplicationID;
				header.messageType = event.m_EventCode;
				header.messageLength = event.m_Data.length();
				header.apiVersion = 0;
				size_t length = sizeof(Header) + header.messageLength;
				char *data = new char[length]();
				std::memcpy(data, &header, sizeof(Header));
				std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
				boost::asio::async_write(m_BoostSocket->m_Socket, boost::asio::buffer(data, length), [this, data](boost::system::error_code ec, size_t) {
					delete[](data);
					if (!ec)
					{
						//m_Handler->OnWrite(m_SessionID); #todo
					}
					else if (!m_Closed.load())
						Close();
					});
			});
		}

		void BoostNetworkUser::Close()
		{
			if (!m_Closed.load())
			{
				LOG(LogType::Standard, '[', GetID(), "] Closing network session");
				boost::asio::post(m_BoostSocket->m_IOContext, [this]() { m_BoostSocket->m_Socket.close(); });
				//m_Handler->CloseSession(m_SessionID); #todo
				m_Closed.store(true);
			}
		}

		BoostNetworkUser::~BoostNetworkUser()
		{
			Close();
			if (m_MessageData)
				delete[](m_MessageData);
			if (m_BoostSocket)
				delete(m_BoostSocket);
		}
	}
}

#endif