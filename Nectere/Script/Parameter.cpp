#include "Script/Parameter.hpp"

#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		Parameter::Parameter(const std::string &name, const VariableType &type, const Parameter::Type &parameterType) : m_Name(name), m_Type(type), m_ParameterType(parameterType), m_DefaultValue(nullptr) {}

		Parameter::Parameter(const Parameter &other) : m_Name(other.m_Name), m_Type(other.m_Type), m_ParameterType(other.m_ParameterType), m_DefaultValue(other.m_DefaultValue) {}

		Parameter &Parameter::operator=(const Parameter &other)
		{
			m_Name = other.m_Name;
			m_Type = other.m_Type;
			m_ParameterType = other.m_ParameterType;
			m_DefaultValue = other.m_DefaultValue;
			return *this;
		}

		void Parameter::AssignDefaultValue(Unit *newDefaultValue)
		{
			m_DefaultValue = newDefaultValue;
		}
	}
}