#pragma once

#include "Script/SubTypeLinker.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_LinkerObjectType, typename t_PrimitiveSubType>
		class PrimitiveLinker final : SubTypeLinker<t_LinkerObjectType>
		{
			using super = SubTypeLinker<t_LinkerObjectType>;
			friend class Linker<t_LinkerObjectType>;
		private:
			t_PrimitiveSubType(t_LinkerObjectType::*m_Getter)() const;
			void (t_LinkerObjectType::*m_Setter)(t_PrimitiveSubType);
			t_PrimitiveSubType t_LinkerObjectType::*m_Raw;

		private:
			PrimitiveLinker(t_PrimitiveSubType(t_LinkerObjectType::*getter)() const, void (t_LinkerObjectType::*setter)(t_PrimitiveSubType)) : SubTypeLinker<t_LinkerObjectType>(getter != nullptr && setter != nullptr), m_Getter(getter), m_Setter(setter), m_Raw(nullptr) {}
			PrimitiveLinker(t_PrimitiveSubType t_LinkerObjectType::*raw) : SubTypeLinker<t_LinkerObjectType>(raw != nullptr), m_Getter(nullptr), m_Setter(nullptr), m_Raw(raw) {}
			Variable Convert(const std::string &name, const t_LinkerObjectType &value, const CurrentEnvironment &) override
			{
				if (m_Getter)
					return Variable(name, (value.*m_Getter)());
				return Variable(name, value.*m_Raw);
			}
			bool Fill(t_LinkerObjectType &value, const Variable &variable) override
			{
				if (variable.Is<t_PrimitiveSubType>())
				{
					if (m_Setter)
						(value.*m_Setter)(static_cast<t_PrimitiveSubType>(variable));
					else
						value.*m_Raw = static_cast<t_PrimitiveSubType>(variable);
					return true;
				}
				return false;
			}
			std::string PrimitiveToString(t_PrimitiveSubType prim)
			{
				if (std::is_same<bool, t_PrimitiveSubType>::value)
					return (prim) ? "true" : "false";
				else if (std::is_same<char, t_PrimitiveSubType>::value || std::is_same<unsigned char, t_PrimitiveSubType>::value)
					return std::to_string(static_cast<int>(prim));
				else
					return std::to_string(prim);
			}
			std::string ToString(const t_LinkerObjectType &value) override
			{
				if (m_Getter)
					return PrimitiveToString((value.*m_Getter)());
				return PrimitiveToString(value.*m_Raw);
			}
		};
	}
}