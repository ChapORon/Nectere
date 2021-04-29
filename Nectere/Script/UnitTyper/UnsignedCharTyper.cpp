#include "Script/UnitTyper/UnsignedCharTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool UnsignedCharTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_UCharValue = static_cast<unsigned char>(std::stoi(value));
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not an uchar");
					return false;
				}
			}

			bool UnsignedCharTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_UCharValue = value->m_UCharValue;
				return true;
			}

			std::string UnsignedCharTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(static_cast<unsigned int>(unit->m_UCharValue));
			}
		}
	}
}