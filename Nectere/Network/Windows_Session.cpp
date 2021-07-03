#ifdef WIN32
#include "Network/Windows_Session.hpp"

#include "Logger.hpp"
#include "NectereEventCode.hpp"
#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	namespace Network
	{
		Windows_Session::Windows_Session(unsigned int id, IEventReceiver *handler, const SOCKET &socket) :
			m_ID(id),
			m_Handler(handler),
			m_Socket(socket),
			m_Closed(false)
		{
			u_long iMode = 1;
			int ret = ioctlsocket(m_Socket, FIONBIO, &iMode);
			if (ret != NO_ERROR)
			{
				LOG(LogType::Error, "Cannot change socket blocking mode for session ", m_ID, ": ", WSAGetLastError());
				Clean();
			}
		}

		Nectere::TaskResult Windows_Session::ReadHeader(Header &header)
		{
			char *data = new char[sizeof(Header)];
			int readLength = ::recv(m_Socket, data, sizeof(Header), 0);
			if (readLength == sizeof(Header))
			{
				LOG(LogType::Verbose, '[', m_ID, "] Decoding header");
				std::memcpy(&header, data, sizeof(Header));
				delete[](data);
				return TaskResult::Success;
			}
			else if (readLength > 0)
			{
				LOG(LogType::Verbose, '[', m_ID, "] Misformatted header: Header is smaller than ", sizeof(Header), " bytes");
				Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "Header is smaller than " + std::to_string(sizeof(Header)) + " bytes" });
				delete[](data);
				return TaskResult::NeedUpdate;
			}
			else if (readLength == WSAEWOULDBLOCK)
			{
				delete[](data);
				return TaskResult::NeedUpdate;
			}
			else if (readLength == 0)
			{
				LOG(LogType::Verbose, '[', m_ID, "] Connection closed by client");
				Clean();
				delete[](data);
				return TaskResult::Fail;
			}
			else
			{
				LOG(LogType::Error, "Cannot read event header for session ", m_ID, ": ", WSAGetLastError());
				Clean();
				delete[](data);
				return TaskResult::Fail;
			}
		}

		Nectere::TaskResult Windows_Session::ReadMessage(const Header &header)
		{
			char *data = new char[header.messageLength + 1];
			LOG(LogType::Verbose, '[', m_ID, "] Header decoded, reading ", header.messageLength, " bytes");
			int readLength = ::recv(m_Socket, data, header.messageLength, 0);
			if (readLength > 0)
			{
				data[readLength] = '\0';
				if (readLength == header.messageLength)
				{
					std::string message = std::string(data);
					Event event = Event{ header.applicationID, header.messageType, message };
					LOG(LogType::Verbose, '[', m_ID, "] Received: \"", event.m_Data, '"');
					m_Handler->OnReceive(m_ID, event);
				}
				else
				{
					Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "Expecting " + std::to_string(header.messageLength) + " bytes where " + std::to_string(readLength) + " bytes where read" });
				}
				delete[](data);
				return TaskResult::Success;
			}
			else if (readLength == WSAEWOULDBLOCK)
			{
				LOG(LogType::Error, "Cannot read event content for session ", m_ID, ": No content to read when ", header.messageLength, " bytes are expected");
				delete[](data);
				Send(Event{ 0, NECTERE_EVENT_MISFORMATTED, "No content to read when " + std::to_string(header.messageLength) + " bytes are expected" });
				return TaskResult::NeedUpdate;
			}
			else if (readLength == 0)
			{
				LOG(LogType::Verbose, '[', m_ID, "] Connection closed by client");
				Clean();
				delete[](data);
				return TaskResult::Fail;
			}
			else
			{
				LOG(LogType::Error, "Cannot read event content for session ", m_ID, ": ", WSAGetLastError());
				Clean();
				delete[](data);
				return TaskResult::Fail;
			}
		}

		Nectere::TaskResult Windows_Session::Write(const Nectere::Event &event)
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
				return TaskResult::Fail;
			}
			delete[](data);
			return TaskResult::Success;
		}

		Nectere::TaskResult Windows_Session::Read()
		{
			if (m_Closed.load())
				return TaskResult::Fail;
			Header header;
			switch (ReadHeader(header))
			{
			case TaskResult::Fail:
				return TaskResult::Fail;
			case TaskResult::NeedUpdate:
				return TaskResult::NeedUpdate;
			case TaskResult::Success:
			{
				if (header.messageLength == 0)
				{
					Event event = Event{ header.applicationID, header.messageType, "" };
					LOG(LogType::Verbose, '[', m_ID, "] Header decoded, no bytes to read");
					m_Handler->OnReceive(m_ID, event);
				}
				else
				{
					switch (ReadMessage(header))
					{
					case TaskResult::Fail:
					case TaskResult::NeedUpdate:
						return TaskResult::Fail;
					case TaskResult::Success:
						return TaskResult::Success;
					}
				}
			}
			}
			return TaskResult::Fail;
		}

		Nectere::TaskResult Windows_Session::Write()
		{
			if (m_Closed.load())
				return TaskResult::Fail;
			if (m_SendBuffer.empty())
				return TaskResult::NeedUpdate;
			Nectere::Event msg;
			{
				std::unique_lock<std::mutex> lock(m_SendBufferMutex);
				msg = m_SendBuffer.front();
				m_SendBuffer.pop();
			}
			return Write(msg);
		}

		void Windows_Session::Send(const Nectere::Event &str)
		{
			std::unique_lock<std::mutex> lock(m_SendBufferMutex);
			m_SendBuffer.push(str);
		}

		void Windows_Session::Clean()
		{
			if (!m_Closed.load())
			{
				closesocket(m_Socket);
				//m_Handler->CloseSession(m_ID); #todo
				m_Closed.store(true);
			}
		}

		void Windows_Session::Close()
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