#include "Script/UnitTyper/ArrayTyper.hpp"

#include <sstream>
#include "Script/CurrentEnvironment.hpp"
#include "Script/Unit.hpp"
#include "StringUtils.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool ArrayTyper::Set(Unit *unit, const std::string &value, bool, const CurrentEnvironment &) const
			{
				size_t valueSize = value.size();
				if (valueSize >= 3 && value[0] == '[' && value[valueSize - 1] == ']')
				{
					std::vector<std::string> subValues;
					int check = StringUtils::SmartSplit(subValues, value.substr(1, valueSize - 2), ",", false);
					switch (check)
					{
					case 1:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid array value \"", value, "\": Closing } doesn't match any {");
						return false;
					}
					case 2:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid array value \"", value, "\": Non closing {");
						return false;
					}
					case 3:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid array value \"", value, "\": Non closing \"");
						return false;
					}
					default:
						break;
					}
					for (const std::string &subValue : subValues)
					{
						(void)subValue;
						//#todo Add sub values as unit
					}
					unit->m_Null = false;
				}
				return true;
			}

			bool ArrayTyper::Set(Unit *unit, const Unit *value, bool, const CurrentEnvironment &) const
			{
				size_t n = 0;
				unit->ClearGroup();
				for (auto *element : value->m_GroupValue)
				{
					if (!unit->Set(n, element))
						return false;
					++n;
				}
				return true;
			}

			std::string ArrayTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				std::stringstream stream;
				stream << '[';
				bool first = true;
				for (auto *element : unit->m_GroupValue)
				{
					if (first)
						first = false;
					else
						stream << ", ";
					if (element)
						stream << element->ToRawString();
					else
						stream << "null";
				}
				stream << ']';
				return stream.str();
			}
		}
	}
}