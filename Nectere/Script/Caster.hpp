#pragma once

#include <string>
#include <unordered_map>
#include "Script/ACaster.hpp"
#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class Caster : public ACaster
		{
			using super = ACaster;
			friend class Interpreter;
			friend class Native;
			friend class Script;
			friend class SingleScriptParser;
		private:
			bool m_IsBidirectional;
			VariableType m_TypeA;
			VariableType m_TypeB;
			std::unordered_map<std::string, std::string> m_AToB;
			std::unordered_map<std::string, std::string> m_BToA;

		private:
			Caster(const Caster &);
			bool Cast(const Unit *, const std::string &, Unit *, const std::string &);
			bool Cast(const Unit *, Unit *) override;
			bool CanCast(const VariableType &, const VariableType &) override;
			const Unit *GetUnitToUse(const Unit *, const std::string &);
			Unit *GetUnitToUse(Unit *, const std::string &);

		public:
			Caster(const std::string &, const std::string &, bool);
			bool AddCastDefinition(const std::string &, const std::string &);
		};
	}
}