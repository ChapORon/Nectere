#pragma once

#include "Script/SubTypeLinker.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_LinkerObjectType>
		class StringLinker final : SubTypeLinker<t_LinkerObjectType>
		{
			using super = SubTypeLinker<t_LinkerObjectType>;
			friend class Linker<t_LinkerObjectType>;
		private:
			const std::string &(t_LinkerObjectType::*m_Getter)() const;
			void (t_LinkerObjectType::*m_Setter)(const std::string &);
			std::string t_LinkerObjectType::*m_Raw;

		private:
			StringLinker(const std::string &(t_LinkerObjectType::*getter)() const, void (t_LinkerObjectType::*setter)(const std::string &)) : SubTypeLinker<t_LinkerObjectType>(getter != nullptr && setter != nullptr), m_Getter(getter), m_Setter(setter), m_Raw(nullptr) {}
			StringLinker(std::string t_LinkerObjectType::*raw) : SubTypeLinker<t_LinkerObjectType>(raw != nullptr), m_Getter(nullptr), m_Setter(nullptr), m_Raw(raw) {}
			Variable Convert(const std::string &name, const t_LinkerObjectType &value, const CurrentEnvironment &) override
			{
				if (m_Getter)
					return Variable(name, (value.*m_Getter)());
				return Variable(name, value.*m_Raw);
			}
			bool Fill(t_LinkerObjectType &value, const Variable &variable) override
			{
				if (variable.Is<std::string>())
				{
					if (m_Setter)
						(value.*m_Setter)(static_cast<std::string>(variable));
					else
						value.*m_Raw = static_cast<std::string>(variable);
					return true;
				}
				return false;
			}
			std::string ToString(const t_LinkerObjectType &value) override
			{
				if (m_Getter)
					return (value.*m_Getter)();
				return '"' + value.*m_Raw + '"';
			}
		};
	}
}