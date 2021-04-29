#include "Script/UnitTyper/BoolTyper.hpp"

#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool BoolTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				if (value == "true")
				{
					unit->m_BoolValue = true;
					unit->m_Null = false;
					return true;
				}
				else if (value == "false")
				{
					unit->m_BoolValue = false;
					unit->m_Null = false;
					return true;
				}
				return false;
			}

			bool BoolTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_BoolValue = value->m_BoolValue;
				return true;
			}

			std::string BoolTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return (unit->m_BoolValue ? "true" : "false");
			}
		}
	}
}