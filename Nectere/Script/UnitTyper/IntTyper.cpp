#include "Script/UnitTyper/IntTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool IntTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_IntValue = std::stoi(value);
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not an int");
					return false;
				}
			}

			bool IntTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_IntValue = value->m_IntValue;
				return true;
			}

			std::string IntTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_IntValue);
			}
		}
	}
}