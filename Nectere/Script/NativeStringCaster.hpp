#pragma once

#include "Script/ACaster.hpp"

namespace Nectere
{
	namespace Script
	{
		class NativeStringCaster : public ACaster
		{
			using super = ACaster;
			friend class Native;
		private:
			bool CanCast(const VariableType &, const VariableType &) override;
			bool Cast(const Unit *, Unit *) override;
		};
	}
}