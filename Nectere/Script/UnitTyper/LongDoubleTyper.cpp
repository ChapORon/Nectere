#include "Script/UnitTyper/LongDoubleTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool LongDoubleTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_LongDoubleValue = std::stold(value);
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not a ldouble");
					return false;
				}
			}

			bool LongDoubleTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_LongDoubleValue = value->m_LongDoubleValue;
				return true;
			}

			std::string LongDoubleTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_LongDoubleValue);
			}
		}
	}
}