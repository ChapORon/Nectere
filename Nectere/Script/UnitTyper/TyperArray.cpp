#include "Script/UnitTyper/TyperArray.hpp"

#include "Script/UnitTyper/ArrayTyper.hpp"
#include "Script/UnitTyper/BoolTyper.hpp"
#include "Script/UnitTyper/CharTyper.hpp"
#include "Script/UnitTyper/DoubleTyper.hpp"
#include "Script/UnitTyper/FloatTyper.hpp"
#include "Script/UnitTyper/IntTyper.hpp"
#include "Script/UnitTyper/LongDoubleTyper.hpp"
#include "Script/UnitTyper/LongLongTyper.hpp"
#include "Script/UnitTyper/LongTyper.hpp"
#include "Script/UnitTyper/ObjectTyper.hpp"
#include "Script/UnitTyper/ShortTyper.hpp"
#include "Script/UnitTyper/StringTyper.hpp"
#include "Script/UnitTyper/UnsignedCharTyper.hpp"
#include "Script/UnitTyper/UnsignedIntTyper.hpp"
#include "Script/UnitTyper/UnsignedLongLongTyper.hpp"
#include "Script/UnitTyper/UnsignedLongTyper.hpp"
#include "Script/UnitTyper/UnsignedShortTyper.hpp"
#include "Script/UnitTyper/VoidTyper.hpp"
#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			bool TyperArray::Set(const EVariableType &type, Unit *unit, const std::string &value, bool setSubType, const CurrentEnvironment &helper) const
			{
				if (type >= EVariableType::Count)
					return false;
				Typer *typer = m_Typers[static_cast<int>(type)];
				if (typer)
				{
					if (typer->Set(unit, value, setSubType, helper))
					{
						unit->m_Null = (value == "null");
						return true;
					}
				}
				unit->m_Null = true;
				return false;
			}

			bool TyperArray::Set(const EVariableType &type, Unit *unit, const Unit *value, bool setSubType, const CurrentEnvironment &helper) const
			{
				if (type >= EVariableType::Count)
					return false;
				Typer *typer = m_Typers[static_cast<int>(type)];
				if (typer)
				{
					if (typer->Set(unit, value, setSubType, helper))
					{
						unit->m_Null = value->m_Null;
						unit->m_Null = false;
						return true;
					}
				}
				unit->m_Null = true;
				return false;
			}

			std::string TyperArray::ToString(const EVariableType &type, const Unit *unit) const
			{
				if (type >= EVariableType::Count)
					return "";
				Typer *typer = m_Typers[static_cast<int>(type)];
				if (typer)
					return typer->ToString(unit);
				return "";
			}

			TyperArray::TyperArray()
			{
				m_Typers[static_cast<int>(EVariableType::Void)] = new VoidTyper();
				m_Typers[static_cast<int>(EVariableType::Group)] = new ArrayTyper();
				m_Typers[static_cast<int>(EVariableType::Array)] = new ArrayTyper();
				m_Typers[static_cast<int>(EVariableType::Bool)] = new BoolTyper();
				m_Typers[static_cast<int>(EVariableType::Char)] = new CharTyper();
				m_Typers[static_cast<int>(EVariableType::UnsignedChar)] = new UnsignedCharTyper();
				m_Typers[static_cast<int>(EVariableType::Short)] = new ShortTyper();
				m_Typers[static_cast<int>(EVariableType::UnsignedShort)] = new UnsignedShortTyper();
				m_Typers[static_cast<int>(EVariableType::Int)] = new IntTyper();
				m_Typers[static_cast<int>(EVariableType::UnsignedInt)] = new UnsignedIntTyper();
				m_Typers[static_cast<int>(EVariableType::Long)] = new LongTyper();
				m_Typers[static_cast<int>(EVariableType::UnsignedLong)] = new UnsignedLongTyper();
				m_Typers[static_cast<int>(EVariableType::LongLong)] = new LongLongTyper();
				m_Typers[static_cast<int>(EVariableType::UnsignedLongLong)] = new UnsignedLongLongTyper();
				m_Typers[static_cast<int>(EVariableType::Float)] = new FloatTyper();
				m_Typers[static_cast<int>(EVariableType::Double)] = new DoubleTyper();
				m_Typers[static_cast<int>(EVariableType::LongDouble)] = new LongDoubleTyper();
				m_Typers[static_cast<int>(EVariableType::String)] = new StringTyper();
				m_Typers[static_cast<int>(EVariableType::Object)] = new ObjectTyper();
			}

			TyperArray::~TyperArray()
			{
				for (Typer *typer : m_Typers)
					delete(typer);
			}
		}
	}
}