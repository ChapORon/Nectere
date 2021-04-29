#pragma once

#include "Script/Callable.hpp"

namespace Nectere
{
	namespace Script
	{
		class CallStack;
		class ResolutionTree;
		class Script;
		class Function final : public Callable
		{
			using super = Callable;
			friend class Script;
			friend class SingleScriptParser;
		private:
			Script *m_Script;
			std::vector<std::string> m_UncompiledBody;
			std::vector<ResolutionTree *> m_Body;

		private:
			Function(Script *);
			bool AddBody(const std::string &);
			bool Compile();
			bool Call(CallStack &) override;
			void Clear();

		public:
			~Function();
		};
	}
}