#include "Script/UnitTyper/UnsignedShortTyper.hpp"

#include "Logger.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool UnsignedShortTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				try
				{
					unit->m_UShortValue = static_cast<unsigned short>(std::stoi(value));
					return true;
				}
				catch (const std::exception &)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", value, "\": not an ushort");
					return false;
				}
			}

			bool UnsignedShortTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				unit->m_UShortValue = value->m_UShortValue;
				return true;
			}

			std::string UnsignedShortTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return std::to_string(unit->m_UShortValue);
			}
		}
	}
}