#include "Script/UnitTyper/LongLongTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool LongLongTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_LongLongValue = std::stoll(value);
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not a llong");
					return false;
				}
			}

			bool LongLongTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_LongLongValue = value->m_LongLongValue;
				return true;
			}

			std::string LongLongTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_LongLongValue);
			}
		}
	}
}