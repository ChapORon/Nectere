#pragma once

#include <vector>
#include "Script/Tag.hpp"
#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		struct VariableDefinition final
		{
			std::string m_Name;
			VariableType m_Type;
			std::string m_DefaultValue;
			std::vector<Tag> m_Tags;
		};
	}
}