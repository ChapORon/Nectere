#include "Script/CallStack.hpp"

#include "Script/CodingStyle.hpp"
#include "Script/LocalScope.hpp"
#include "Script/Native.hpp"
#include "Script/Parameter.hpp"
#include "Script/Script.hpp"
#include "Script/Variable.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		CallStack::CallStack(Native *native, Script *current) : m_Native(native), m_Current(current)
		{
			OpenScope();
		}

		CallStack::CallStack(Native *native) : CallStack(native, nullptr) {}

		CallStack::CallStack(Native *native, const std::vector<Parameter> &parameters, const std::vector<Variable> &variables) : CallStack(native, nullptr, parameters, variables) {}

		CallStack::CallStack(Native *native, Script *current, const std::vector<Parameter> &parameters, const std::vector<Variable> &variables) : CallStack(native, current)
		{
			size_t idx = 0;
			size_t variableSize = variables.size();
			for (const auto &parameter : parameters)
			{
				if (idx != variableSize)
				{
					switch (parameter.GetParameterType())
					{
					case Parameter::Type::Mutable:
					{
						m_Parameters[parameter.GetName()] = variables[idx].GetMutable();
						break;
					}
					case Parameter::Type::Const:
					{
						m_Parameters[parameter.GetName()] = variables[idx].GetMutable();
						m_Parameters[parameter.GetName()].SetConstness(true);
						break;
					}
					case Parameter::Type::Copy:
					default:
					{
						m_Parameters[parameter.GetName()] = variables[idx].GetCopy();
						break;
					}
					}
					++idx;
				}
				else
					m_Parameters[parameter.GetName()] = Variable(parameter.GetDefaultValue(), false);
			}
		}

		bool CallStack::CreateVar(const std::string &variableName, const std::string &variableType, const std::string &variableValue, const std::vector<Tag> &variableTags)
		{
			if (!CodingStyle::CheckName(CodingStyle::NameType::Variable, variableName))
				return false;
			for (const auto &locals : m_Locals)
			{
				if (locals->Have(variableName))
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "A local variable with the name ", variableName, " already exist");
					return false;
				}
			}
			if (m_Parameters.find(variableName) != m_Parameters.end())
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "A parameter with the name ", variableName, " already exist");
				return false;
			}
			else if (m_Native->HaveGlobal(variableName) || (m_Current && m_Current->HaveGlobal(variableName, true, false)))
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "A global variable with the name ", variableName, " already exist");
				return false;
			}
			if (m_Native->CheckType(variableType))
			{
				m_Locals[0]->Add(variableName, m_Native->CreateVar(variableName, variableType, variableValue, variableTags));
				return true;
			}
			else if (m_Current && m_Current->CheckType(variableType, false))
			{
				m_Locals[0]->Add(variableName, m_Current->CreateVar(variableName, variableType, variableValue, false, variableTags));
				return true;
			}
			LOG_SCRIPT_RUNTIME(LogType::Error, "Unknown type ", variableType);
			return false;
		}

		void CallStack::SetReturn(Variable &&ret)
		{
			m_Return = ret;
		}

		bool CallStack::OpenScope()
		{
			LocalScope *newScope = new LocalScope();
			if (newScope)
			{
				m_Locals.insert(m_Locals.begin(), newScope);
				return true;
			}
			LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create new scope: Out of memory");
			return false;
		}

		void CallStack::CloseScope()
		{
			delete(*m_Locals.erase(m_Locals.begin()));
		}

		Variable CallStack::operator[](const std::string &name)
		{
			for (const auto &locals : m_Locals)
			{
				if (locals->Have(name))
					return locals->Get(name);
			}
			auto it = m_Parameters.find(name);
			if (it != m_Parameters.end())
				return (*it).second.GetMutable();
			else if (m_Native->HaveGlobal(name))
				return m_Native->GetGlobal(name);
			else if (m_Current)
				return m_Current->GetGlobal(name, false);
			return Variable::NULL_VARIABLE;
		}

		CallStack::~CallStack()
		{
			for (auto *locals : m_Locals)
				delete(locals);
		}
	}
}