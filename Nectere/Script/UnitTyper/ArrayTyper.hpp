#pragma once

#include "Script/UnitTyper/Typer.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			class ArrayTyper final : public Typer
			{
				using super = Typer;
			private:
				bool Set(Unit *, const std::string &, bool, const CurrentEnvironment &) const override;
				bool Set(Unit *, const Unit *, bool, const CurrentEnvironment &) const override;
				std::string ToString(const Unit *) const final override;

			public:
				ArrayTyper() = default;
				virtual ~ArrayTyper() = default;
			};
		}
	}
}