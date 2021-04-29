#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Script/Taggable.hpp"
#include "Script/VariableDefinition.hpp"

namespace Nectere
{
	namespace Script
	{
		class Unit;
		class DeclaredObjectDefinition final : public Taggable
		{
			friend class Native;
			template <typename t_LinkerType>
			friend class Linker;
			friend class Script;
			friend class SingleScriptParser;
			friend class Unit;
		public:
			typedef std::vector<VariableDefinition>::iterator iterator;
			typedef std::vector<VariableDefinition>::const_iterator const_iterator;

		private:
			std::string m_TypeName;
			std::unordered_set<std::string> m_SubTypes;
			std::vector<VariableDefinition> m_Variables;
			std::unordered_map<std::string, DeclaredObjectDefinition *> m_SubTypesDefinition;

		private:
			DeclaredObjectDefinition() = default;
			bool AddVariable(const std::string &, const VariableType &, const std::string &, const std::vector<Tag> &);
			inline bool AddVariable(const std::string &, const VariableType &, const std::vector<Tag> &);
			void AddDefinitionOf(DeclaredObjectDefinition *);
			DeclaredObjectDefinition *GetDefinitionOf(const std::string &) const;

		public:
			inline iterator begin() { return m_Variables.begin(); }
			inline const_iterator begin() const { return m_Variables.begin(); }
			inline iterator end() { return m_Variables.end(); }
			inline const_iterator end() const { return m_Variables.end(); }
		};
	}
}