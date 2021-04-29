#pragma once

#include <string>
#include "Script/EVariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class VariableType final
		{
		public:
			static const VariableType invalid;
		private:
			EVariableType m_Type;
			VariableType *m_ArrayType;
			std::string m_Name;

		public:
			VariableType();
			VariableType(const VariableType &);
			VariableType(const EVariableType &);
			VariableType(const EVariableType &, const VariableType &);
			VariableType(const EVariableType &, const EVariableType &);
			VariableType(const std::string &);
			inline const EVariableType &GetType() const { return m_Type; }
			std::string GetName() const;
			inline const VariableType *GetArrayType() const { return m_ArrayType; }
			bool SetArrayType(const VariableType &);
			inline bool operator==(const EVariableType &type) const { return type == m_Type; }
			inline bool operator!=(const EVariableType &type) const { return type != m_Type; }
			inline bool operator==(const std::string &name) const { return name == m_Name; }
			inline bool operator!=(const std::string &name) const { return name != m_Name; }
			inline bool operator==(const VariableType &other) const
			{
				return (m_Type == other.m_Type && m_Name == other.m_Name && (m_ArrayType == other.m_ArrayType || (m_ArrayType != nullptr && other.m_ArrayType != nullptr && *m_ArrayType == *other.m_ArrayType)));
			}
			inline bool operator!=(const VariableType &other) const { return !(*this == other); }
			~VariableType();
		};

		struct VariableTypeHasher
		{
			std::size_t operator()(const VariableType &) const noexcept;
		};
	}
}