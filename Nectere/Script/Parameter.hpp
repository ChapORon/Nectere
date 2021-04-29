#pragma once

#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class Unit;
		class Parameter final
		{
			friend class Signature;
		public:
			enum class Type : int
			{
				Copy,
				Mutable,
				Const,
				Count,
				Invalid
			};
		private:
			std::string m_Name;
			VariableType m_Type;
			Parameter::Type m_ParameterType;
			Unit *m_DefaultValue;

		private:
			void AssignDefaultValue(Unit *);

		public:
			Parameter(const Parameter &);
			Parameter &operator=(const Parameter &);
			Parameter(const std::string &, const VariableType &, const Parameter::Type &);
			inline const std::string &GetName() const { return m_Name; }
			inline const VariableType &GetType() const { return m_Type; }
			inline Parameter::Type GetParameterType() const { return m_ParameterType; }
			inline bool HaveDefaultValue() const { return m_DefaultValue != nullptr; }
			inline Unit *GetDefaultValue() const { return m_DefaultValue; }
		};
	}
}