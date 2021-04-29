#include "Script/UnitTyper/UnsignedLongLongTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool UnsignedLongLongTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_ULongLongValue = std::stoull(value);
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not an ullong");
					return false;
				}
			}

			bool UnsignedLongLongTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_ULongLongValue = value->m_ULongLongValue;
				return true;
			}

			std::string UnsignedLongLongTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_ULongLongValue);
			}
		}
	}
}