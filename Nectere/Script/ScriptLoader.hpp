#pragma once

#include "Script/Script.hpp"

namespace Nectere
{
	namespace Script
	{
		class ScriptLoader final
		{
			friend class DeclaredObjectDefinition;
			friend class Interpreter;
			friend class Script;
			friend class SingleScriptParser;
		private:
			std::unordered_map<std::string, Script *> m_LoadedScript;

		private:
			Script *Load(const std::string &, bool, Native *);
			Script *Get(const std::string &);
			Script *Load(const std::string &, Native *);
			bool Reload(const std::string &, Native *);

		public:
			ScriptLoader() = default;
			~ScriptLoader();
		};
	}
}