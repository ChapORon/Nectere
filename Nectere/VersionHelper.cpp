#include "VersionHelper.hpp"

#include <sstream>

namespace Nectere
{
	uint16_t VersionHelper::ms_Major = 0;
	uint16_t VersionHelper::ms_Minor = 0;
	uint16_t VersionHelper::ms_Patch = 0;

	void VersionHelper::SetupEvent(Event &event)
	{
		event.m_APIVersion.m_Major = ms_Major;
		event.m_APIVersion.m_Minor = ms_Minor;
		event.m_APIVersion.m_Patch = ms_Patch;
	}

	std::string VersionHelper::GetVersion()
	{
		std::stringstream stream;
		stream << ms_Major << '.' << ms_Minor << '.' << ms_Patch;
		return stream.str();
	}

	void VersionHelper::SetVersion(const std::string &version)
	{
		if (::sscanf(version.c_str(), "%hu.%hu.%hu", &ms_Major, &ms_Minor, &ms_Patch) != 3 || ms_Major > 99 || ms_Minor > 99 || ms_Patch > 99)
		{
			ms_Major = 0;
			ms_Minor = 0;
			ms_Patch = 0;
		}
	}
}