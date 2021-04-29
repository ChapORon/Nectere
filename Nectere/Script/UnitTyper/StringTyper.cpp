#include "Script/UnitTyper/StringTyper.hpp"

#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool StringTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				size_t valueSize = value.size();
				if (value.find('"') == std::string::npos)
				{
					if (valueSize != 0)
					{
						unit->m_StringValue = value;
						return true;
					}
				}
				else if (value[0] == '"' && value[valueSize - 1] == '"')
				{
					if (valueSize >= 3)
					{
						unit->m_StringValue = value.substr(1, valueSize - 2);
						return true;
					}
				}
				return false;
			}

			bool StringTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_StringValue = value->m_StringValue;
				return true;
			}

			std::string StringTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return '"' + unit->m_StringValue + '"';
			}
		}
	}
}