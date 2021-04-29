#pragma once

#include "Script/SubTypeLinker.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_LinkerObjectType, typename t_ObjectSubType>
		class ObjectLinker final : SubTypeLinker<t_LinkerObjectType>
		{
			using super = SubTypeLinker<t_LinkerObjectType>;
			friend class Linker<t_LinkerObjectType>;
		private:
			const t_ObjectSubType &(t_LinkerObjectType::*m_ConstGetter)() const;
			t_ObjectSubType &(t_LinkerObjectType::*m_Getter)();
			t_ObjectSubType t_LinkerObjectType::*m_Raw;
			const Linker<t_ObjectSubType> &m_SubTypeObjectLinker;

		private:
			ObjectLinker(const t_ObjectSubType &(t_LinkerObjectType::*constGetter)() const, t_ObjectSubType &(t_LinkerObjectType::*getter)(), const Linker<t_ObjectSubType> &subTypeObjectLinker) : SubTypeLinker<t_LinkerObjectType>(constGetter != nullptr && getter != nullptr), m_ConstGetter(constGetter), m_Getter(getter), m_Raw(nullptr), m_SubTypeObjectLinker(subTypeObjectLinker) {}
			ObjectLinker(t_ObjectSubType t_LinkerObjectType::*raw, const Linker<t_ObjectSubType> &subTypeObjectLinker) : SubTypeLinker<t_LinkerObjectType>(raw != nullptr), m_ConstGetter(nullptr), m_Getter(nullptr), m_Raw(raw), m_SubTypeObjectLinker(subTypeObjectLinker) {}

			Variable Convert(const std::string &name, const t_LinkerObjectType &value, const CurrentEnvironment &helper) override
			{
				if (m_ConstGetter)
					return m_SubTypeObjectLinker.Convert(name, (value.*m_ConstGetter)(), helper);
				return m_SubTypeObjectLinker.Convert(name, value.*m_Raw, helper);

			}

			bool Fill(t_LinkerObjectType &value, const Variable &variable) override
			{
				if (m_Getter)
					return m_SubTypeObjectLinker.Fill(variable, (value.*m_Getter)());
				else
					return m_SubTypeObjectLinker.Fill(variable, value.*m_Raw);
			}

			std::string ToString(const t_LinkerObjectType &value) override
			{
				if (m_ConstGetter)
					return m_SubTypeObjectLinker.ToString((value.*m_ConstGetter)());
				return m_SubTypeObjectLinker.ToString(value.*m_Raw);
			}
		};
	}
}