#pragma once

#include "Script/Taggable.hpp"

namespace Nectere
{
	namespace Script
	{
		class Unit;
		class VariableType;
		class ACaster : public Taggable
		{
			friend class CurrentEnvironment;
			friend class Interpreter;
			friend class Native;
			friend class Script;
		private:
			virtual bool Cast(const Unit *, Unit *) = 0;
			virtual bool CanCast(const VariableType &, const VariableType &) = 0;

		public:
			virtual ~ACaster() = default;
		};
	}
}