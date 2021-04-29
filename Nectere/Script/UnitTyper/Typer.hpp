#pragma once

#include <string>

namespace Nectere
{
	namespace Script
	{
		class CurrentEnvironment;
		class Unit;
		namespace UnitTyper
		{
			class TyperArray;
			class Typer
			{
				friend class TyperArray;
			private:
				virtual bool Set(Unit *, const std::string &, bool, const CurrentEnvironment &) const = 0;
				virtual bool Set(Unit *, const Unit *, bool, const CurrentEnvironment &) const = 0;
				virtual std::string ToString(const Unit *) const = 0;

			public:
				virtual ~Typer() = default;
			};
		}
	}
}