#include "Script/Interpreter.hpp"

#include "Script/Caster.hpp"
#include "Script/Caster.hpp"
#include "Script/ScriptLoader.hpp"

namespace Nectere
{
	namespace Script
	{
		Interpreter::Interpreter() : m_Native(new Native()), m_ScriptLoader(new ScriptLoader()), m_Current(nullptr)
		{
			if (!m_Native)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create new native handler: Out of memory");
			if (!m_ScriptLoader)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create new script loader: Out of memory");
		}

		Interpreter::Interpreter(const std::vector<std::string> &vec) : m_Native(new Native()), m_ScriptLoader(new ScriptLoader()), m_Current(nullptr)
		{
			if (!m_Native)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create new native handler: Out of memory");
			if (!m_ScriptLoader)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create new script loader: Out of memory");
		}

		Interpreter::Interpreter(int argc, char **args) : Interpreter(std::vector<std::string>(args, args + argc)) {}

		bool Interpreter::Load(const std::string &path, bool useLoadedScript)
		{
			if (!m_ScriptLoader)
				return false;
			Script *loadedScript = m_ScriptLoader->Load(path, m_Native);
			if (!loadedScript)
				return false;
			if (useLoadedScript)
				m_Current = loadedScript;
			return true;
		}

		bool Interpreter::Use(const std::string &path)
		{
			if (!m_ScriptLoader)
				return false;
			Script *tmp = m_ScriptLoader->Get(path);
			if (!tmp)
				return false;
			m_Current = tmp;
			return true;
		}

		bool Interpreter::Reload(const std::string &path)
		{
			if (!m_ScriptLoader)
				return false;
			return m_ScriptLoader->Reload(path, m_Native);
		}

		Variable Interpreter::CreateVar(const std::string &variableName, const std::string &variableType, const std::string &variableValue, const std::vector<Tag> &tags)
		{
			if (!m_ScriptLoader)
				return Variable::NULL_VARIABLE;
			if (m_Native && m_Native->CheckType(variableType))
				return m_Native->CreateVar(variableName, variableType, variableValue, tags);
			if (m_Current)
				return m_Current->CreateVar(variableName, variableType, variableValue, false, tags);
			return Variable::NULL_VARIABLE;
		}

		Variable Interpreter::GetGlobal(const std::string &name)
		{
			if (!m_ScriptLoader)
				return Variable::NULL_VARIABLE;
			if (m_Native && m_Native->HaveGlobal(name))
				return m_Native->GetGlobalVariable(name);
			if (m_Current)
				return m_Current->GetGlobal(name, false);
			return Variable::NULL_VARIABLE;
		}

		Callable *Interpreter::GetCallable(const std::string &name)
		{
			if (!m_ScriptLoader)
				return nullptr;
			if (m_Native && m_Native->HaveCallable(name))
				return m_Native->GetCallable(name);
			if (m_Current)
				return m_Current->GetCallable(name, false, false);
			return nullptr;
		}

		DeclaredObjectDefinition *Interpreter::GetDeclaredObject(const std::string &name)
		{
			if (!m_ScriptLoader)
				return nullptr;
			if (m_Native)
			{
				if (DeclaredObjectDefinition *nativeDefinition = m_Native->GetObjectDefinition(name))
					return nativeDefinition;
			}
			if (m_Current)
				return m_Current->GetObjectDefinition(name, false, false);
			return nullptr;
		}

		Interpreter::~Interpreter()
		{
			if (m_Native)
				delete(m_Native);
			if (m_ScriptLoader)
				delete(m_ScriptLoader);
		}

		bool Interpreter::RegisterCaster(const Caster &caster)
		{
			if (Caster *newCaster = new Caster(caster))
				return m_Native->AddCaster(newCaster);
			else
			{
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create caster: Out of memory");
				return false;
			}
		}

		Variable Interpreter::Cast(const Variable &from, const VariableType &type)
		{
			if (type.GetType() != EVariableType::Invalid)
			{
				Variable to = CreateVar(from.GetName(), type.GetName(), from.GetTagList());
				if (m_Native)
				{
					if (ACaster *caster = m_Native->GetCaster(from.GetVariableType(), to.GetVariableType()))
					{
						if (caster->Cast(from.m_Unit, to.m_Unit))
							return to;
					}
				}
				if (m_Current)
				{
					if (ACaster *caster = m_Current->GetCaster(from.GetVariableType(), to.GetVariableType(), true, false))
					{
						if (caster->Cast(from.m_Unit, to.m_Unit))
							return to;
					}
				}
			}
			return Variable::NULL_VARIABLE;
		}

		Variable Interpreter::Cast(const Variable &from, const EVariableType &type)
		{
			return Cast(from, VariableType(type));
		}

		Variable Interpreter::Cast(const Variable &from, const std::string &type)
		{
			return Cast(from, VariableType(type));
		}
	}
}