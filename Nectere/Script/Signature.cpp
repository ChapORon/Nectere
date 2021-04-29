#include "Script/Signature.hpp"

#include "Logger.hpp"
#include "Script/CurrentEnvironment.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		void Signature::AddArgument(const std::string &name, const VariableType &type, const Parameter::Type &parameterType)
		{
			Parameter parameter(name, type, parameterType);
			m_Arguments.emplace_back(parameter);
		}

		Signature::Signature() : m_ReturnType(EVariableType::Void) {}

		bool Signature::CompileParametersDefault(const CurrentEnvironment &helper, const std::unordered_map<std::string, std::string> &defaultValues)
		{
			for (auto &parameter : m_Arguments)
			{
				auto it = defaultValues.find(parameter.GetName());
				if (it != defaultValues.end())
				{
					std::string defaultValue = (*it).second;
					const VariableType &parameterType = parameter.GetType();
					Unit *newUnit;
					if (parameterType == EVariableType::Object)
					{
						std::string parameterTypeName = parameterType.GetName();
						if (DeclaredObjectDefinition *declaredObjectDefinition = helper.GetObjectDefinition(parameterTypeName))
							newUnit = new Unit(declaredObjectDefinition, defaultValue, helper);
						else
						{
							LOG_SCRIPT_COMPILATION(LogType::Error, "Cannot compile function ", m_Name, ": Unknown parameter type ", parameterTypeName, " for ", parameter.GetName());
							return false;
						}
					}
					else
					{
						newUnit = new Unit(parameterType, defaultValue, helper);
					}
					if (!newUnit)
					{
						LOG_SCRIPT_COMPILATION(LogType::Error, "Cannot compile function ", m_Name, ": Out of memory ");
						return false;
					}
					LOG_SCRIPT_COMPILATION(LogType::Verbose, "Assigning default value ", defaultValue, " to parameter ", parameter.GetName(), " in function ", m_Name);
					parameter.AssignDefaultValue(newUnit);
				}
			}
			return true;
		}

		Signature::~Signature()
		{
			for (auto &parameter : m_Arguments)
			{
				if (parameter.m_DefaultValue)
					delete(parameter.m_DefaultValue);
			}
		}
	}
}