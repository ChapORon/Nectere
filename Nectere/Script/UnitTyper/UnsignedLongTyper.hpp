#pragma once

#include "Script/UnitTyper/Typer.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			class UnsignedLongTyper final : public Typer
			{
				using super = Typer;
			private:
				bool Set(Unit *, const std::string &, bool, const CurrentEnvironment &) const override;
				bool Set(Unit *, const Unit *, bool, const CurrentEnvironment &) const override;
				std::string ToString(const Unit *) const override;

			public:
				UnsignedLongTyper() = default;
				virtual ~UnsignedLongTyper() = default;
			};
		}
	}
}