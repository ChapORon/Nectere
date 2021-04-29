#pragma once

#include <vector>
#include "Script/IResolutionTreeOperation.hpp"

namespace Nectere
{
	namespace Script
	{
		class Script;
		class ResolutionTree final
		{
			friend class Function;
		protected:
			bool m_HasResolve;
			IResolutionTreeOperation *m_Operation;
			std::vector<ResolutionTree *> m_Childs;

		private:
			ResolutionTree(IResolutionTreeOperation *);
			ResolutionTree(IResolutionTreeOperation *, const std::vector<ResolutionTree *> &);
			Variable ResolveChild(Script *, CallStack &);
			bool Resolve(Script *, CallStack &);

		public:
			~ResolutionTree();
		};
	}
}