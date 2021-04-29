#include "Script/UnitTyper/ObjectTyper.hpp"

#include <sstream>
#include "Logger.hpp"
#include "Script/Unit.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool ObjectTyper::Set(Unit *unit, const std::string &constValue, bool setSubType, const CurrentEnvironment &) const
			{
				std::string value = constValue;
				size_t openBracketPos = value.find('{');
				if (openBracketPos == std::string::npos)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid object definition \"", constValue, "\": No { in definition");
					return false;
				}
				if (openBracketPos != 0)
				{
					value = value.substr(openBracketPos);
					std::string explicitTypeName = constValue.substr(0, openBracketPos - 1);
					if (unit->m_Type.GetName() != explicitTypeName)
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid object definition \"", constValue, "\": Explicit name ", explicitTypeName, " does not match");
						return false;
					}
				}
				size_t valueSize = value.size();
				if (valueSize >= 3 && value[0] == '{' && value[valueSize - 1] == '}')
				{
					std::vector<std::string> subValues;
					int check = StringUtils::SmartSplit(subValues, value.substr(1, valueSize - 2), ",", false);
					switch (check)
					{
					case 1:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid object definition \"", constValue, "\": Closing } doesn't match any {");
						return false;
					}
					case 2:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid object definition \"", constValue, "\": Non closing {");
						return false;
					}
					case 3:
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid object definition \"", constValue, "\": Non closing \"");
						return false;
					}
					default:
						break;
					}
					size_t subValuesSize = subValues.size();
					size_t n = 0;
					for (const auto &variable : *unit->m_Definition)
					{
						auto it = unit->m_Child.find(variable.m_Name);
						if (it != unit->m_Child.end())
						{
							if (n < subValuesSize)
							{
								auto *subValueToSet = (*it).second;
								const auto &subValue = subValues[n];
								if (subValue.size() != 0 && (subValueToSet->m_Type.GetType() != EVariableType::Object || setSubType))
								{
									if (!subValueToSet->Set(subValue))
										return false;
								}
							}
						}
						++n;
					}
					unit->m_Null = false;
				}
				return true;
			}

			bool ObjectTyper::Set(Unit *unit, const Unit *value, bool setSubType, const CurrentEnvironment &) const
			{
				for (const auto &variable : *unit->m_Definition)
				{
					auto it = unit->m_Child.find(variable.m_Name);
					auto valueIt = value->m_Child.find(variable.m_Name);
					if (it != unit->m_Child.end() && valueIt != value->m_Child.end())
					{
						auto *subValueToSet = (*it).second;
						auto *subValue = (*valueIt).second;
						if (subValue != nullptr && (subValueToSet->m_Type.GetType() != EVariableType::Object || setSubType))
						{
							if (!subValueToSet->Set(subValue))
								return false;
						}
					}
				}
				return true;
			}

			std::string ObjectTyper::ToString(const Unit *unit) const
			{
				std::stringstream stream;
				stream << '{';
				bool first = true;
				for (const auto &variable : *unit->m_Definition)
				{
					if (first)
						first = false;
					else
						stream << ", ";
					auto it = unit->m_Child.find(variable.m_Name);
					if (it != unit->m_Child.end())
						stream << (*it).second->ToString();
				}
				stream << '}';
				return stream.str();
			}
		}
	}
}