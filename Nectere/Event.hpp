#pragma once

#include <string>

namespace Nectere
{
	struct Version
	{
		uint16_t m_Major;
		uint16_t m_Minor;
		uint16_t m_Patch;
	};

	struct Event
	{
		bool m_Error;
		uint16_t m_CanalID;
		uint16_t m_ApplicationID;
		uint16_t m_UserID;
		uint16_t m_EventCode;
		Version m_APIVersion;
		std::string m_Data;
	};
}