#include "Script/DeclaredObjectDefinition.hpp"

#include "Script/CodingStyle.hpp"
#include "Script/ScriptLoader.hpp"
#include "Script/VariableDefinition.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{

		void DeclaredObjectDefinition::AddDefinitionOf(DeclaredObjectDefinition *subTypeDefinition)
		{
			if (subTypeDefinition)
				m_SubTypesDefinition[subTypeDefinition->m_TypeName] = subTypeDefinition;
		}

		DeclaredObjectDefinition *DeclaredObjectDefinition::GetDefinitionOf(const std::string &type) const
		{
			auto it = m_SubTypesDefinition.find(type);
			if (it == m_SubTypesDefinition.end())
				return nullptr;
			return (*it).second;
		}

		bool DeclaredObjectDefinition::AddVariable(const std::string &name, const VariableType &type, const std::string &defaultValue, const std::vector<Tag> &tags)
		{
			if (!CodingStyle::CheckName(CodingStyle::NameType::Variable, name))
				return false;
			if (type == EVariableType::Object && type.GetName().size() != 0)
				m_SubTypes.insert(type.GetName());
			m_Variables.emplace_back(VariableDefinition{ name, type, defaultValue, tags });
			return true;
		}
		bool DeclaredObjectDefinition::AddVariable(const std::string &name, const VariableType &type, const std::vector<Tag> &tags)
		{
			return AddVariable(name, type, "", tags);
		}
	}
}