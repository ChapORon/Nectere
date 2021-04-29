#pragma once

#include "Script/Parameter.hpp"
#include "Script/Script.hpp"
#include "Script/NativeUtils.hpp"
#include "Script/Variable.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		class Caster;
		class ScriptLoader;
		class Interpreter final
		{
		private:
			Native *m_Native;
			NativeUtils m_NativeUtils;
			ScriptLoader *m_ScriptLoader;
			Script *m_Current;
			std::vector<Variable> m_LastVariables;
			std::vector<Parameter> m_LastParameters;

		private:
			template <typename t_TypeToMutate>
			bool InternalMutate(size_t parameter, t_TypeToMutate &&value)
			{
				if (m_LastParameters[parameter].GetParameterType() == Parameter::Type::MUTABLE)
					return m_NativeUtils.Fill(m_Native, m_LastVariables[parameter], value);
				return true;
			}
			template <typename t_GlobalType>
			inline bool InternalRegister(const std::string &name, const t_GlobalType &value, const std::vector<Tag> &tags) { return (m_Native) ? m_NativeUtils.AddGlobal<t_GlobalType>(m_Native, name, tags, value) : false; }
			template <typename ...t_Arguments>
			bool InternalCall(const std::string &functionToCall, Variable &ret, t_Arguments &&...args)
			{
				if (!m_ScriptLoader || !m_Native)
					return false;
				Callable *callable;
				if (m_Current)
					callable = m_Current->GetCallable(functionToCall, false, true);
				else
					callable = m_Native->GetCallable(functionToCall);
				if (callable)
				{
					m_LastParameters.clear();
					m_LastVariables.clear();
					m_LastParameters = callable->GetParameters();
					(m_LastVariables.emplace_back(m_NativeUtils.Convert(m_Native, "", std::forward<t_Arguments>(args))), ...);
					if (!callable->CheckParameter(m_LastVariables))
					{
						m_LastParameters.clear();
						m_LastVariables.clear();
						return false;
					}
					CallStack stack(m_Native, m_Current, m_LastParameters, m_LastVariables);
					if (!(*callable)(stack))
						return false;
					ret = stack.GetReturn();
					return true;
				}
				LOG_SCRIPT_RUNTIME(LogType::CRASH, "Unknown function ", functionToCall);
				return false;
			}
			Callable *GetCallable(const std::string &);
			DeclaredObjectDefinition *GetDeclaredObject(const std::string &);

		public:
			Interpreter();
			Interpreter(int argc, char **args);
			Interpreter(const std::vector<std::string> &);
			bool Load(const std::string &, bool = false);
			bool Use(const std::string &);
			bool Reload(const std::string &);
			Variable CreateVar(const std::string &, const std::string &, const std::string &, const std::vector<Tag> &);
			inline Variable CreateVar(const std::string &name, const std::string &type, const std::string &value) { return CreateVar(name, type, value, std::vector<Tag>()); }
			inline Variable CreateVar(const std::string &name, const std::string &type, const std::vector<Tag> &tags) { return CreateVar(name, type, "", tags); }
			inline Variable CreateVar(const std::string &name, const std::string &type) { return CreateVar(name, type, "", std::vector<Tag>()); }
			Variable GetGlobal(const std::string &);
			Variable Cast(const Variable &, const VariableType &);
			Variable Cast(const Variable &, const EVariableType &);
			Variable Cast(const Variable &, const std::string &);
			template <typename t_TypeToCastTo>
			bool Cast(const Variable &from, t_TypeToCastTo &value)
			{
				if (!m_Native)
					return false;
				Variable to = Cast(from, NativeUtils::TypeOf(m_Native));
				if (to.IsNull())
					return false;
				return NativeUtils::Fill(m_Native, to, value);
			}
			template <typename t_TypeOfGlobal>
			bool GetGlobal(const std::string &globalName, t_TypeOfGlobal &value)
			{
				if (!m_Native)
					return false;
				Variable global = GetGlobal(globalName);
				if (global.IsNull())
					return false;
				return NativeUtils::Fill(m_Native, global, value);
			}
			template <typename ...t_Arguments>
			bool CallVoid(const std::string &functionToCall, t_Arguments &&...args)
			{
				Variable ret;
				return InternalCall(functionToCall, ret, std::forward<t_Arguments>(args)...);
			}
			template <typename ...t_Arguments>
			Variable Call(const std::string &functionToCall, t_Arguments &&...args)
			{
				Variable returnVariable;
				if (!InternalCall(functionToCall, returnVariable, std::forward<t_Arguments>(args)...))
					return Variable::NULL_VARIABLE;
				return returnVariable;
			}
			template <typename t_TypeOfReturn, typename ...t_Arguments>
			bool CallInto(const std::string &functionToCall, t_TypeOfReturn &ret, t_Arguments &&...args)
			{
				if (!m_Native)
					return false;
				Variable returnVariable;
				if (!InternalCall(functionToCall, returnVariable, std::forward<t_Arguments>(args)...))
					return false;
				return m_NativeUtils.Fill(m_Native, returnVariable, ret);
			}

			template <typename t_TypeOfReturn>
			bool Mutate(size_t parameter, t_TypeOfReturn &value)
			{
				if (m_LastVariables.size() <= parameter || !m_Native)
					return false;
				if (m_LastParameters[parameter].GetParameterType() == Parameter::Type::MUTABLE)
					return m_NativeUtils.Fill(m_Native, m_LastVariables[parameter], value);
				return false;
			}
			//==========Registration of C++ declared object linkers==========
			template <typename t_LinkerType>
			inline bool RegisterLinker(Linker<t_LinkerType> &linker) { return (m_Native) ? m_NativeUtils.RegisterLinker<t_LinkerType>(m_Native, linker) : false; }

			//==========Registration of C++ globals==========
			template <typename ...t_GlobalElements>
			inline bool RegisterGroup(const std::string &name, t_GlobalElements &&...args) { return (m_Native) ? m_NativeUtils.AddGlobal<t_GlobalElements...>(m_Native, name, std::vector<Tag>(), std::forward<t_GlobalElements>(args)...) : false; }
			template <typename t_GlobalType>
			inline bool RegisterArray(const std::string &name, const std::vector<t_GlobalType> &value) { return (m_Native) ? m_NativeUtils.AddGlobal<t_GlobalType>(m_Native, name, std::vector<Tag>(), value) : false; }
			template <typename t_GlobalType>
			inline bool RegisterGlobal(const std::string &name, const t_GlobalType &value) { return InternalRegister<t_GlobalType>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, bool value) { return InternalRegister<bool>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, char value) { return InternalRegister<char>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, unsigned char value) { return InternalRegister<unsigned char>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, short value) { return InternalRegister<short>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, unsigned short value) { return InternalRegister<unsigned short>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, int value) { return InternalRegister<int>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, unsigned int value) { return InternalRegister<unsigned int>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, long value) { return InternalRegister<long>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, unsigned long value) { return InternalRegister<unsigned long>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, long long value) { return InternalRegister<long long>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, unsigned long long value) { return InternalRegister<unsigned long long>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, float value) { return InternalRegister<float>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, double value) { return InternalRegister<double>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, long double value) { return InternalRegister<long double>(name, value, std::vector<Tag>()); }
			inline bool RegisterGlobal(const std::string &name, std::string value) { return InternalRegister<std::string>(name, value, std::vector<Tag>()); }
			template <typename ...t_GlobalElements>
			inline bool RegisterGroup(const std::string &name, const std::vector<Tag> &tags, t_GlobalElements &&...args) { return (m_Native) ? m_NativeUtils.AddGlobal<t_GlobalElements...>(m_Native, name, tags, std::forward<t_GlobalElements>(args)...) : false; }
			template <typename t_GlobalType>
			inline bool RegisterArray(const std::string &name, const std::vector<Tag> &tags, const std::vector<t_GlobalType> &value) { return (m_Native) ? m_NativeUtils.AddGlobal<t_GlobalType>(m_Native, name, tags, value) : false; }
			template <typename t_GlobalType>
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, const t_GlobalType &value) { return InternalRegister<t_GlobalType>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, bool value) { return InternalRegister<bool>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, char value) { return InternalRegister<char>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, unsigned char value) { return InternalRegister<unsigned char>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, short value) { return InternalRegister<short>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, unsigned short value) { return InternalRegister<unsigned short>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, int value) { return InternalRegister<int>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, unsigned int value) { return InternalRegister<unsigned int>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, long value) { return InternalRegister<long>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, unsigned long value) { return InternalRegister<unsigned long>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, long long value) { return InternalRegister<long long>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, unsigned long long value) { return InternalRegister<unsigned long long>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, float value) { return InternalRegister<float>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, double value) { return InternalRegister<double>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, long double value) { return InternalRegister<long double>(name, value, tags); }
			inline bool RegisterGlobal(const std::string &name, const std::vector<Tag> &tags, std::string value) { return InternalRegister<std::string>(name, value, tags); }

			//==========Registration of C++ functions==========
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, const t_ReturnType &(*function)(CallStack &)) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, std::vector<Tag>()) : false; }
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, t_ReturnType &(*function)(CallStack &)) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, std::vector<Tag>()) : false; }
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, t_ReturnType(*function)(CallStack &)) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, std::vector<Tag>()) : false; }
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, const t_ReturnType &(*function)(CallStack &), const std::vector<Tag> &tags) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, tags) : false; }
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, t_ReturnType &(*function)(CallStack &), const std::vector<Tag> &tags) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, tags) : false; }
			template <typename t_ReturnType>
			inline bool RegisterFunction(const std::string &signature, t_ReturnType(*function)(CallStack &), const std::vector<Tag> &tags) { return (m_Native) ? m_NativeUtils.RegisterCallable<t_ReturnType>(m_Native, signature, function, tags) : false; }

			//==========Registration of C++ caster==========
			bool RegisterCaster(const Caster &);
			~Interpreter();
		};
	}
}