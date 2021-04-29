#pragma once

#include <unordered_map>
#include <vector>
#include "Script/Parameter.hpp"

namespace Nectere
{
	namespace Script
	{
		class Callable;
		class CurrentEnvironment;
		class Signature final
		{
			friend class Callable;
		private:
			VariableType m_ReturnType;
			std::string m_Name;
			std::vector<Parameter> m_Arguments;

		private:
			void AddArgument(const std::string &, const VariableType &, const Parameter::Type &);
			bool CompileParametersDefault(const CurrentEnvironment &, const std::unordered_map<std::string, std::string> &);

		public:
			inline const VariableType &GetReturnType() const { return m_ReturnType; }
			inline const std::string &GetName() const { return m_Name; }
			inline const std::vector<Parameter> &GetParameters() const { return m_Arguments; }
			Signature();
			~Signature();
		};
	}
}