#ifdef WIN32
#include "Network/WindowsNetworkUser.hpp"

#include "Logger.hpp"
#include "Command/NectereEventCode.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	namespace Network
	{
		WindowsNetworkUser::WindowsNetworkUser(const SOCKET &socket) :
			m_Socket(socket),
			m_Closed(false)
		{
			u_long iMode = 1;
			int ret = ioctlsocket(m_Socket, FIONBIO, &iMode);
			if (ret != NO_ERROR)
			{
				LOG(LogType::Error, "Cannot change socket blocking mode for session ", GetID(), ": ", WSAGetLastError());
				Clean();
			}
		}

        Concurrency::TaskResult WindowsNetworkUser::ReadHeader(Header &header)
		{
			char *data = new char[sizeof(Header)];
			int readLength = ::recv(m_Socket, data, sizeof(Header), 0);
			if (readLength == sizeof(Header))
			{
				LOG(LogType::Verbose, '[', GetID(), "] Decoding header");
				std::memcpy(&header, data, sizeof(Header));
				delete[](data);
				return Concurrency::TaskResult::Success;
			}
			else if (readLength > 0)
			{
				LOG(LogType::Verbose, '[', GetID(), "] Misformated header: Header is smaller than ", sizeof(Header), " bytes");
				Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "Header is smaller than " + std::to_string(sizeof(Header)) + " bytes" });
				delete[](data);
				return Concurrency::TaskResult::NeedUpdate;
			}
			else if (readLength == WSAEWOULDBLOCK)
			{
				delete[](data);
				return Concurrency::TaskResult::NeedUpdate;
			}
			else if (readLength == 0)
			{
				LOG(LogType::Verbose, '[', GetID(), "] Connection closed by client");
				Clean();
				delete[](data);
				return Concurrency::TaskResult::Fail;
			}
			else
			{
				LOG(LogType::Error, "Cannot read event header for session ", GetID(), ": ", WSAGetLastError());
				Clean();
				delete[](data);
				return Concurrency::TaskResult::Fail;
			}
		}

        Concurrency::TaskResult WindowsNetworkUser::ReadMessage(const Header &header)
		{
			char *data = new char[header.messageLength + 1];
			LOG(LogType::Verbose, '[', GetID(), "] Header decoded, reading ", header.messageLength, " bytes");
			int readLength = ::recv(m_Socket, data, header.messageLength, 0);
			if (readLength > 0)
			{
				data[readLength] = '\0';
				if (readLength == header.messageLength)
				{
					std::string message = std::string(data);
					Event event = Event{ header.applicationID, header.messageType, message };
					LOG(LogType::Verbose, '[', GetID(), "] Received: \"", event.m_Data, '"');
					//m_Handler->OnReceive(GetID(), event);
				}
				else
				{
					Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "Expecting " + std::to_string(header.messageLength) + " bytes where " + std::to_string(readLength) + " bytes where read" });
				}
				delete[](data);
				return Concurrency::TaskResult::Success;
			}
			else if (readLength == WSAEWOULDBLOCK)
			{
				LOG(LogType::Error, "Cannot read event content for session ", GetID(), ": No content to read when ", header.messageLength, " bytes are expected");
				delete[](data);
				Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "No content to read when " + std::to_string(header.messageLength) + " bytes are expected" });
				return Concurrency::TaskResult::NeedUpdate;
			}
			else if (readLength == 0)
			{
				LOG(LogType::Verbose, '[', GetID(), "] Connection closed by client");
				Clean();
				delete[](data);
				return Concurrency::TaskResult::Fail;
			}
			else
			{
				LOG(LogType::Error, "Cannot read event content for session ", GetID(), ": ", WSAGetLastError());
				Clean();
				delete[](data);
				return Concurrency::TaskResult::Fail;
			}
		}

        Concurrency::TaskResult WindowsNetworkUser::Write(const Nectere::Event &event)
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
			int ret = ::send(m_Socket, data, length, 0);
			if (ret == SOCKET_ERROR)
			{
				Clean();
				delete[](data);
				return Concurrency::TaskResult::Fail;
			}
			delete[](data);
			return Concurrency::TaskResult::Success;
		}

        Concurrency::TaskResult WindowsNetworkUser::Read()
		{
			if (m_Closed.load())
				return Concurrency::TaskResult::Fail;
			Header header;
			switch (ReadHeader(header))
			{
			case Concurrency::TaskResult::Fail:
				return Concurrency::TaskResult::Fail;
			case Concurrency::TaskResult::NeedUpdate:
				return Concurrency::TaskResult::NeedUpdate;
			case Concurrency::TaskResult::Success:
			{
				if (header.messageLength == 0)
				{
					Event event = Event{ header.applicationID, header.messageType, "" };
					LOG(LogType::Verbose, '[', GetID(), "] Header decoded, no bytes to read");
					Send(event);
				}
				else
				{
					switch (ReadMessage(header))
					{
					case Concurrency::TaskResult::Fail:
					case Concurrency::TaskResult::NeedUpdate:
						return Concurrency::TaskResult::Fail;
					case Concurrency::TaskResult::Success:
						return Concurrency::TaskResult::Success;
					}
				}
			}
			}
			return Concurrency::TaskResult::Fail;
		}

        Concurrency::TaskResult WindowsNetworkUser::Write()
		{
			if (m_Closed.load())
				return Concurrency::TaskResult::Fail;
			if (m_SendBuffer.empty())
				return Concurrency::TaskResult::NeedUpdate;
			Nectere::Event msg;
			{
				std::unique_lock<std::mutex> lock(m_SendBufferMutex);
				msg = m_SendBuffer.front();
				m_SendBuffer.pop();
			}
			return Write(msg);
		}

		void WindowsNetworkUser::Send(const Nectere::Event &str)
		{
			std::unique_lock<std::mutex> lock(m_SendBufferMutex);
			m_SendBuffer.push(str);
		}

		void WindowsNetworkUser::Clean()
		{
			if (!m_Closed.load())
			{
				closesocket(m_Socket);
				//m_Handler->CloseSession(GetID()); #todo
				m_Closed.store(true);
			}
		}

		void WindowsNetworkUser::Close()
		{
			if (!m_Closed.load())
			{
				shutdown(m_Socket, SD_SEND);
				Clean();
			}
		}
	}
}

#endif