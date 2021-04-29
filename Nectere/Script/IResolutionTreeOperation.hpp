#pragma once

#include <vector>
#include "Script/CallStack.hpp"

namespace Nectere
{
	namespace Script
	{
		class IResolutionTreeOperation
		{
			friend class ResolutionTree;
		private:
			virtual Variable Call(Script *, CallStack &, const std::vector<Variable> &, bool &) = 0;
			virtual Variable Call(Script *, CallStack &, bool &) = 0;
			virtual ~IResolutionTreeOperation() = default;
		};
	}
}