#include "Script/UnitTyper/VoidTyper.hpp"

#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool VoidTyper::Set(Unit *, const std::string &, bool, const CurrentEnvironment &) const { return true; }

			bool VoidTyper::Set(Unit *, const Unit *, bool, const CurrentEnvironment &) const { return true; }

			std::string VoidTyper::ToString(const Unit *unit) const
			{
				if (unit->m_Null)
					return "null";
				return "";
			}
		}
	}
}