#pragma once

#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class VariableTypeHelper
		{
		public:
			static bool IsRaw(const VariableType &);
			static bool IsRaw(const std::string &);
			static bool IsNotObject(const VariableType &);
			static bool IsNotObject(const std::string &);
		};
	}
}