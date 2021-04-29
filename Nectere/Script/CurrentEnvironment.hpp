#pragma once

#include <string>
#include <vector>
#include "Script/Tag.hpp"

namespace Nectere
{
	namespace Script
	{
		class Callable;
		class DeclaredObjectDefinition;
		class Native;
		class Script;
		class Unit;
		class Variable;
		class CurrentEnvironment final
		{
		private:
			Native *m_Native;
			Script *m_Script;

		public:
			CurrentEnvironment(Native *);
			CurrentEnvironment(Script *);
			DeclaredObjectDefinition *GetObjectDefinition(const std::string &) const;
			Variable GetGlobal(const std::string &) const;
			Unit *CreateVar(const std::string &, const std::string &, const std::string &, const std::vector<Tag> &) const;
			Callable *GetCallable(const std::string &) const;
			bool Cast(const Unit *, Unit *) const;
			bool Cast(const Variable &, Variable &) const;
		};
	}
}