#pragma once

#include <unordered_map>
#include "Script/Tag.hpp"
#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class Callable;
		class DeclaredObjectDefinition;
		class Script;
		class ScriptLoader;
		class SingleScriptParser
		{
			friend class NativeUtils;
			friend class ScriptLoader;
		private:
			std::string m_Content;
			Script *m_Script;
			ScriptLoader *m_Loader;
			std::unordered_set<std::string> m_RegisteredImportToCompile;
			std::unordered_set<std::string> m_RegisteredDefinitionToCompile;
			std::unordered_set<std::string> m_RegisteredGlobalToCompile;
			std::unordered_set<std::string> m_RegisteredFunctionToCompile;
			std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> m_RegisteredCasterToCompile;

		private:
			static bool ParseSignature(const std::string &, bool &, Callable *);

		private:
			SingleScriptParser(const std::string &, Script *, ScriptLoader *);
			bool TreatTags(const std::string &, size_t &, std::vector<Tag> &);
			DeclaredObjectDefinition *ParseDeclaredObjectDefinition(const std::string &);
			bool ParseGlobal(const std::string &, const std::vector<Tag> &);
			bool ParseImport(const std::string &, const std::string &, const std::vector<Tag> &);
			bool ParseDeclaredObject(const std::string &, const std::vector<Tag> &);
			bool ParseFunction(const std::string &, const std::vector<Tag> &);
			bool ParseCast(const std::string &, const std::vector<Tag> &, bool);
			bool ParsePublicCast(const std::string &, const std::vector<Tag> &);
			bool ParsePrivateCast(const std::string &, const std::vector<Tag> &);
			bool Parse(const std::string &);
		};
	}
}