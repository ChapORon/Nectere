#pragma once

#include <array>
#include "Script/EVariableType.hpp"
#include "Script/UnitTyper/Typer.hpp"

namespace Nectere
{
	namespace Script
	{
		class Unit;
		namespace UnitTyper
		{
			class TyperArray
			{
				friend class Unit;
			private:
				std::array<Typer *, EVARIABLETYPE_SIZE> m_Typers;

			private:
				bool Set(const EVariableType &, Unit *, const std::string &, bool, const CurrentEnvironment &) const;
				bool Set(const EVariableType &, Unit *, const Unit *, bool, const CurrentEnvironment &) const;
				std::string ToString(const EVariableType &, const Unit *) const;

			public:
				TyperArray();
				~TyperArray();
			};
		}
	}
}