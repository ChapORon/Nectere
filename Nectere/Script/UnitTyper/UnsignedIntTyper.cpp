#include "Script/UnitTyper/UnsignedIntTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool UnsignedIntTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_UIntValue = static_cast<unsigned int>(std::stoi(value));
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not an uint");
					return false;
				}
			}

			bool UnsignedIntTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_UIntValue = value->m_UIntValue;
				return true;
			}

			std::string UnsignedIntTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_UIntValue);
			}
		}
	}
}