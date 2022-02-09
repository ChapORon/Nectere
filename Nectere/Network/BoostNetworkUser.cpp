#ifdef USE_BOOST

#include "Network/BoostNetworkUser.hpp"
#include "Network/Helper.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Network
	{
		BoostNetworkUser::BoostNetworkUser(const Logger *logger, boost::asio::io_context &ioContext, boost::asio::ip::tcp::socket socket) :
			m_Closed(false),
			m_Logger(logger),
			m_IOContext(ioContext),
			m_Socket(std::move(socket))
		{
			Read();
		}

		void BoostNetworkUser::Read()
		{
			char *data = new char[sizeof(Header)]();
			boost::asio::async_read(m_Socket, boost::asio::buffer(data, sizeof(Header)), [=](boost::system::error_code ec, size_t length) mutable {
				if (!ec && length == sizeof(Header))
				{
					Header header;
					Log(LogType::Standard, '[', GetID(), "] Decoding header");
					std::memcpy(&header, data, sizeof(Header));
					delete[](data);
					Log(LogType::Standard, '[', GetID(), "] Header decoded, reading ", header.messageLength, " bytes");
					data = nullptr;
					if (header.messageLength > 0)
					{
						if (data = new char[header.messageLength + 1])
						{
							size_t length = boost::asio::read(m_Socket, boost::asio::buffer(data, header.messageLength));
							if (length != header.messageLength)
							{
								Close();
								return;
							}
							data[length] = '\0';
						}
					}
					Event event = Helper::Convert(header, data);
					if (data)
						delete[](data);
					Log(LogType::Standard, '[', GetID(), "] Received {app:", event.m_ApplicationID, ", command:", event.m_EventCode, ", canal:", event.m_CanalID, ", data:\"", event.m_Data, "\"}");
					Send(event);
					if (!m_Closed.load())
						Read();
				}
				else if (!m_Closed.load())
					Close();
				});
		}

		void BoostNetworkUser::Receive(const Event &event)
		{
			boost::asio::post(m_IOContext, [this, event]() {
				Header header = Helper::GenerateHeader(event);
				size_t length = sizeof(Header) + header.messageLength;
				char *data = new char[length]();
				std::memcpy(data, &header, sizeof(Header));
				std::memcpy(&data[sizeof(Header)], event.m_Data.data(), event.m_Data.length());
				Log(LogType::Debug, "Sending ", length, " bytes with ", sizeof(Header), " of header and ", header.messageLength, " bytes of data");
				boost::asio::async_write(m_Socket, boost::asio::buffer(data, length), [this, data](boost::system::error_code ec, size_t) {
					delete[](data);
					if (ec && !m_Closed.load())
						Close();
					});
			});
		}

		void BoostNetworkUser::OnClose()
		{
			if (!m_Closed.load())
			{
				Log(LogType::Standard, '[', GetID(), "] Closing network session");
				boost::asio::post(m_IOContext, [this]() {
					m_Socket.close();
					MarkForDelete();
				});
				m_Closed.store(true);
			}
		}

		BoostNetworkUser::~BoostNetworkUser()
		{
			Close();
		}
	}
}

#endif