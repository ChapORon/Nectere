#include "Network/Helper.hpp"

namespace Nectere
{
	namespace Network
	{
		Event Helper::Convert(const Header &header, const char *data)
		{
			Event event;
			std::string message;
			uint16_t version = header.apiVersion;
			if (data)
				message = std::string(data);
			event.m_ApplicationID = header.applicationID;
			event.m_UserID = header.userID;
			event.m_EventCode = header.messageType;
			event.m_APIVersion.m_Patch = version % 100;
			version /= 100;
			event.m_APIVersion.m_Minor = version % 100;
			version /= 100;
			event.m_APIVersion.m_Major = version % 100;
			event.m_Data = message;
			event.m_CanalID = header.canalID;
			event.m_Error = static_cast<bool>(header.error);
			return event;
		}

		Header Helper::GenerateHeader(const Event &event)
		{
			Header header;
			header.canalID = event.m_CanalID;
			header.error = static_cast<uint8_t>(event.m_Error);
			header.applicationID = event.m_ApplicationID;
			header.userID = event.m_UserID;
			header.messageType = event.m_EventCode;
			header.apiVersion = (((event.m_APIVersion.m_Major * 100) + event.m_APIVersion.m_Minor) * 100) + event.m_APIVersion.m_Patch;
			header.messageLength = event.m_Data.length();
			return header;
		}

		void Helper::LogHeaderInfo(const Logger *logger)
		{
			if (logger)
			{
				logger->Log(LogType::Debug, "Header is ", sizeof(Header), " bytes dispatch in:");
				logger->Log(LogType::Debug, "- ", sizeof(Header::error), " bytes for the error marker");
				logger->Log(LogType::Debug, "- ", sizeof(Header::canalID), " bytes for the ID of the canal the message came from (if applicable)");
				logger->Log(LogType::Debug, "- ", sizeof(Header::applicationID), " bytes for the ID of the application the message came from");
				logger->Log(LogType::Debug, "- ", sizeof(Header::userID), " bytes for the ID of the user the message came from");
				logger->Log(LogType::Debug, "- ", sizeof(Header::messageType), " bytes for the type of command");
				logger->Log(LogType::Debug, "- ", sizeof(Header::apiVersion), " bytes for the Nectere API version use to send this event");
				logger->Log(LogType::Debug, "- ", sizeof(Header::messageLength), " bytes for the size of the data following the header");
				logger->Log(LogType::Debug, "- ", sizeof(Header) - (sizeof(Header::error) + sizeof(Header::canalID) + sizeof(Header::applicationID) +
					sizeof(Header::userID) + sizeof(Header::messageType) + sizeof(Header::apiVersion) + sizeof(Header::messageLength)), " bytes of padding");
			}
		}
	}
}