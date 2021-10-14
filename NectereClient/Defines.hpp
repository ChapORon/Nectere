#pragma once

namespace NectereClient
{
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
}