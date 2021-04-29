#pragma once

#include <string>

namespace Nectere
{
	namespace Script
	{
		class ResolutionTree;
		class ResolutionTreeUtils final
		{
			friend class Function;
		private:
			static ResolutionTree *Compile(const std::string &);
		};
	}
}