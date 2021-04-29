#include "Script/VariableTypeHelper.hpp"

namespace Nectere
{
	namespace Script
	{
		bool VariableTypeHelper::IsRaw(const VariableType &variableType)
		{
			const EVariableType &type = variableType.GetType();
			return (type == EVariableType::Bool || type == EVariableType::Char || type == EVariableType::UnsignedChar ||
				type == EVariableType::Short || type == EVariableType::UnsignedShort || type == EVariableType::Int ||
				type == EVariableType::UnsignedInt || type == EVariableType::Long || type == EVariableType::UnsignedLong ||
				type == EVariableType::LongLong || type == EVariableType::UnsignedLongLong || type == EVariableType::Float ||
				type == EVariableType::Double || type == EVariableType::LongDouble || type == EVariableType::String);
		}

		bool VariableTypeHelper::IsRaw(const std::string &typeName)
		{
			VariableType typeTocheck(typeName);
			return IsRaw(typeTocheck);
		}

		bool VariableTypeHelper::IsNotObject(const VariableType &variableType)
		{
			const EVariableType &type = variableType.GetType();
			return (IsRaw(variableType) || type == EVariableType::Array || type == EVariableType::Group);
		}

		bool VariableTypeHelper::IsNotObject(const std::string &typeName)
		{
			VariableType typeTocheck(typeName);
			return IsNotObject(typeTocheck);
		}
	}
}