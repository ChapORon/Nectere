#pragma once

#include <string>
#include <unordered_set>

namespace Nectere
{
	namespace Script
	{
		struct Tag
		{
			std::string m_Name;
			std::unordered_set<std::string> m_Params;
		};
	}
}