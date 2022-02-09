#pragma once

#include "Event.hpp"

namespace Nectere
{
	class VersionHelper
	{
	private:
		static uint16_t ms_Major;
		static uint16_t ms_Minor;
		static uint16_t ms_Patch;

	public:
		static void SetupEvent(Event &);
		static std::string GetVersion();
		static void SetVersion(const std::string &);
	};
}