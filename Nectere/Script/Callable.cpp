#include "Script/Callable.hpp"

#include "Script/CodingStyle.hpp"
#include "Script/CurrentEnvironment.hpp"
#include "Script/CallStack.hpp"
#include "Script/Variable.hpp"
#include "Logger.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		bool Callable::operator()(CallStack &parameters)
		{
			return Call(parameters);
		}

		bool Callable::CheckParameter(const std::vector<Variable> &variables) const
		{
			const std::vector<Parameter> &parameters = m_Signature.m_Arguments;
			size_t parametersSize = parameters.size();
			size_t variableSize = variables.size();
			if (variableSize > parametersSize)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Too much argument given to ", m_Signature.m_Name);
				return false;
			}
			size_t idx = 0;
			for (const auto &parameter : parameters)
			{
				if (idx != variableSize)
				{
					const VariableType &variableType = variables[idx].GetVariableType();
					const VariableType &parameterType = parameter.GetType();
					if (variableType != parameterType)
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Invalid parameter ", idx, ": Parameter is of type ", variableType.GetName(), " instead of ", parameterType.GetName());
						return false;
					}
					++idx;
				}
				else if (!parameter.HaveDefaultValue())
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Not enough argument given to ", m_Signature.m_Name);
					return false;
				}
			}
			return true;
		}

		bool Callable::Compile(const CurrentEnvironment &helper)
		{
			if (!CheckSignature() || !m_Signature.CompileParametersDefault(helper, m_DefaultValues))
				return false;
			return true;
		}

		void Callable::SetReturnType(const VariableType &type)
		{
			m_Signature.m_ReturnType = type;
		}

		void Callable::SetName(const std::string &name)
		{
			m_Signature.m_Name = name;
		}

		void Callable::AddDefaultValue(const std::string &parameter, const std::string &defaultValue)
		{
			m_DefaultValues[parameter] = defaultValue;
		}

		void Callable::AddArgument(const std::string &name, const VariableType &type, const Parameter::Type &parameterType)
		{
			m_Signature.AddArgument(name, type, parameterType);
		}
	}
}