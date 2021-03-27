#pragma once

#include <string>

namespace Nectere
{
	struct Event
	{
		uint16_t m_ApplicationID;
		uint16_t m_EventCode;
		std::string m_Data;
	};
}