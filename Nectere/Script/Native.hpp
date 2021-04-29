#pragma once

#include <unordered_map>
#include <unordered_set>
#include "Script/CurrentEnvironment.hpp"
#include "Script/VariableType.hpp"

namespace Nectere
{
	namespace Script
	{
		class Callable;
		class Caster;
		class DeclaredObjectDefinition;
		class ACaster;
		class ILinkerWrapper;
		class Unit;
		class Variable;
		class Native final
		{
			friend class CurrentEnvironment;
			friend class CallStack;
			friend class Interpreter;
			template <typename t_LinkerType>
			friend class Linker;
			friend class NativeUtils;
			friend class Script;

		private:
			const CurrentEnvironment &m_Helper;
			std::unordered_set<std::string> m_RegisteredTypes;
			std::unordered_map<size_t, ILinkerWrapper *> m_Serializers;
			std::unordered_map<std::string, DeclaredObjectDefinition *> m_DeclaredObjectDefinition;
			std::unordered_map<std::string, Unit *> m_Globals;
			std::unordered_map<std::string, Callable *> m_Functions;
			std::unordered_map<VariableType, std::vector<Caster *>, VariableTypeHasher> m_Casters;
			std::unordered_map<EVariableType, ACaster *> m_NativeCasters;
			std::unordered_map<std::string, std::pair<size_t, Callable *>> m_UnCompiledFunctions;

		private:
			Native();
			bool CheckType(const VariableType &, bool);
			bool CheckType(const std::string &);
			bool AddObjectDefinition(DeclaredObjectDefinition *);
			bool AddCallable(Callable *);
			bool AddCallable(size_t, Callable *);
			bool AddCaster(Caster *);
			DeclaredObjectDefinition *GetObjectDefinition(const std::string &);
			ACaster *GetCaster(const VariableType &, const VariableType &);
			Unit *GetGlobal(const std::string &);
			Variable GetGlobalVariable(const std::string &);
			Callable *GetCallable(const std::string &);
			Unit *CreateUnit(const std::string &, const std::string &, const std::string &, const std::vector<Tag> &);
			Variable CreateVar(const std::string &, const std::string &, const std::string &, const std::vector<Tag> &);
			bool HaveObjectDefinition(const std::string &);
			bool HaveGlobal(const std::string &);
			bool HaveCallable(const std::string &);
			bool CanCast(const VariableType &, const VariableType &);
			~Native();
		};
	}
}