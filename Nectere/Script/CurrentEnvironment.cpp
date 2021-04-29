#include "Script/CurrentEnvironment.hpp"

#include "Script/ACaster.hpp"
#include "Script/Native.hpp"
#include "Script/Script.hpp"
#include "Script/Unit.hpp"
#include "Script/Variable.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		CurrentEnvironment::CurrentEnvironment(Native *native) : m_Native(native), m_Script(nullptr) {}
		CurrentEnvironment::CurrentEnvironment(Script *script) : m_Native(nullptr), m_Script(script) {}

		DeclaredObjectDefinition *CurrentEnvironment::GetObjectDefinition(const std::string &name) const
		{
			if (m_Script)
				return m_Script->GetObjectDefinition(name, true, true);
			return m_Native->GetObjectDefinition(name);
		}

		Variable CurrentEnvironment::GetGlobal(const std::string &name) const
		{
			if (m_Script)
				return m_Script->GetGlobal(name, true);
			return m_Native->GetGlobalVariable(name);
		}

		Callable *CurrentEnvironment::GetCallable(const std::string &name) const
		{
			if (m_Script)
				return m_Script->GetCallable(name, true, true);
			return m_Native->GetCallable(name);
		}

		Unit *CurrentEnvironment::CreateVar(const std::string &variableName, const std::string &variableTypeName, const std::string &variableValue, const std::vector<Tag> &tags) const
		{
			if (m_Script)
				return m_Script->CreateVar(Nectere::Logger::ScriptEngine::runtime, variableName, variableTypeName, variableValue, true, tags);
			return m_Native->CreateUnit(variableName, variableTypeName, variableValue, tags);
		}

		bool CurrentEnvironment::Cast(const Unit *from, Unit *to) const
		{
			if (from == nullptr)
			{
				to->m_Null = true;
				return true;
			}
			else if (to == nullptr)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot cast: Receiver unit is null");
				return false;
			}
			if (m_Script)
			{
				if (ACaster *caster = m_Script->GetCaster(from->m_Type, to->m_Type, true, true))
					return caster->Cast(from, to);
			}
			else
			{
				if (ACaster *caster = m_Native->GetCaster(from->m_Type, to->m_Type))
					return caster->Cast(from, to);
			}
			return false;
		}

		bool CurrentEnvironment::Cast(const Variable &from, Variable &to) const
		{
			return Cast(from.m_Unit, to.m_Unit);
		}
	}
}