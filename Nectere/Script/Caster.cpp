#include "Script/Caster.hpp"

#include "Script/CurrentEnvironment.hpp"
#include "Script/Unit.hpp"
#include "Script/VariableTypeHelper.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		Caster::Caster(const std::string &typeA, const std::string &typeB, bool bidirectionnal) : m_IsBidirectional(bidirectionnal), m_TypeA(typeA), m_TypeB(typeB) {}

		Caster::Caster(const Caster &other) : m_IsBidirectional(other.m_IsBidirectional), m_TypeA(other.m_TypeA), m_TypeB(other.m_TypeB), m_AToB(other.m_AToB), m_BToA(other.m_BToA)
		{
			AddTags(&other);
		}

		bool Caster::AddCastDefinition(const std::string &from, const std::string &to)
		{
			if (from.length() == 0)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot add cast definition \"", from, " : ", to, "\": From is empty");
				return false;
			}
			else if (to.length() == 0)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot add cast definition \"", from, " : ", to, "\": To is empty");
				return false;
			}
			m_AToB[from] = to;
			if (m_IsBidirectional)
				m_BToA[to] = from;
			return true;
		}

		bool Caster::Cast(const Unit *from, Unit *to)
		{
			if (from->m_Type == m_TypeA && to->m_Type == m_TypeB)
			{
				for (const auto &pair : m_AToB)
				{
					if (!Cast(from, pair.first, to, pair.second))
						return false;
				}
				to->m_Null = from->m_Null;
				return true;
			}
			else if (m_IsBidirectional && from->m_Type == m_TypeB && to->m_Type == m_TypeA)
			{
				for (const auto &pair : m_BToA)
				{
					if (!Cast(from, pair.first, to, pair.second))
						return false;
				}
				to->m_Null = from->m_Null;
				return true;
			}
			return false;
		}

		bool Caster::Cast(const Unit *from, const std::string &fromKey, Unit *to, const std::string &toKey)
		{
			if (const Unit *valueToUse = GetUnitToUse(from, fromKey))
			{
				if (Unit *valueToSet = GetUnitToUse(to, toKey))
					return valueToSet->Set(valueToUse);
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot cast ", from->m_Type.GetName(), " into ", to->m_Type.GetName(), ": Unknown subtype ", toKey);
				return false;
			}
			LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot cast ", from->m_Type.GetName(), " into ", to->m_Type.GetName(), ": Unknown subtype ", fromKey);
			return false;
		}

		bool Caster::CanCast(const VariableType &from, const VariableType &to)
		{
			if ((from == m_TypeA && to == m_TypeB) || (m_IsBidirectional && from == m_TypeB && to == m_TypeA))
				return true;
			return false;
		}

		Unit *Caster::GetUnitToUse(Unit *unit, const std::string &key)
		{
			size_t keySize = key.size();
			if (key[0] == '[' && key[keySize - 1] == ']')
			{
				if (keySize == 2)
					return unit;
				else
				{
					size_t idx = static_cast<size_t>(std::stoull(key.substr(1, keySize - 2)));
					return unit->Get(idx);
				}
			}
			else
				return unit->Get(key);
		}

		const Unit *Caster::GetUnitToUse(const Unit *unit, const std::string &key)
		{
			size_t keySize = key.size();
			if (key[0] == '[' && key[keySize - 1] == ']')
			{
				if (keySize == 2)
					return unit;
				else
				{
					size_t idx = static_cast<size_t>(std::stoull(key.substr(1, keySize - 2)));
					return unit->Get(idx);
				}
			}
			else
				return unit->c_Get(key);
		}
	}
}