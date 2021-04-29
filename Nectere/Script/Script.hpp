#pragma once

#include <atomic>
#include <vector>
#include "Logger.hpp"
#include "Script/Native.hpp"
#include "Script/Taggable.hpp"

namespace Nectere
{
	namespace Script
	{
		class ScriptLoader;
		class Function;
		class Script final : public Taggable
		{
			friend class CurrentEnvironment;
			friend class CallStack;
			friend class Interpreter;
			friend class ScriptLoader;
			friend class SingleScriptParser;
		private:
			enum class ScriptInnerCall : int
			{
				Compile,
				GetObjectDefinition,
				GetGlobal,
				GetCallable,
				GetCaster,
				HaveObjectDefinition,
				HaveGlobal,
				HaveCallable,
				HaveCaster,
				CheckDuplicateObjectDefinition,
				CheckDuplicateGlobal,
				CheckDuplicateCallable,
				CheckDuplicateCaster,
				Count,
				Invalid
			};
			class TreeLocker;
			class TreeLock final
			{
				friend class TreeLocker;
				static constexpr unsigned int SCRIPT_INNER_CALL_SIZE = static_cast<unsigned int>(ScriptInnerCall::Count);
			private:
				std::atomic_bool m_Locks[SCRIPT_INNER_CALL_SIZE];

			private:
				bool Lock(ScriptInnerCall);
				void Unlock(ScriptInnerCall);

			public:
				TreeLock();
			};
			class TreeLocker final
			{
			private:
				TreeLock *m_Lock;
				ScriptInnerCall m_LockType;

			public:
				TreeLocker(TreeLock *lock, const ScriptInnerCall &lockType) : m_Lock(lock), m_LockType(lockType) {}
				inline bool Lock() { return m_Lock->Lock(m_LockType); }
				~TreeLocker() { m_Lock->Unlock(m_LockType); }
			};
			struct GlobalDefinition
			{
				std::string m_Name;
				std::string m_TypeName;
				std::string m_Value;
				bool m_IsPrivate;
				std::vector<Tag> m_Tags;
			};
		private:
			const CurrentEnvironment &m_Helper;
			TreeLock m_TreeLock;
			std::string m_ScriptName;
			bool m_Compiled;
			//Native elements (implemented through the Interpreter)
			Native *m_Native;
			//Imports
			std::vector<Script *> m_AllImports;
			std::vector<Script *> m_PrivateImports;
			std::vector<Script *> m_Imports;
			//Declared object
			std::unordered_map<std::string, DeclaredObjectDefinition *> m_PrivateDeclaredObjectDefinition;
			std::unordered_map<std::string, DeclaredObjectDefinition *> m_DeclaredObjectDefinition;
			//Globals
			std::unordered_map<std::string, Unit *> m_PrivateGlobals;
			std::unordered_map<std::string, Unit *> m_Globals;
			std::vector<GlobalDefinition> m_GlobalToCompile;
			//Functions
			std::unordered_map<std::string, Callable *> m_PrivateFunctions;
			std::unordered_map<std::string, Callable *> m_Functions;
			//Casters
			std::unordered_map<VariableType, std::vector<Caster *>, VariableTypeHasher> m_PrivateCasters;
			std::unordered_map<VariableType, std::vector<Caster *>, VariableTypeHasher> m_Casters;

		private:
			Script(Native *, const std::string &);
			bool CheckType(const VariableType &, bool, bool);
			bool CheckType(const std::string &, bool);

			void AddGlobalToCompile(const GlobalDefinition &);
			void AddPrivateImport(Script *);
			void AddPublicImport(Script *);
			void AddPrivateDeclaredObjectDefinition(DeclaredObjectDefinition *);
			void AddPublicDeclaredObjectDefinition(DeclaredObjectDefinition *);
			void AddPrivateFunction(Function *);
			void AddPublicFunction(Function *);
			void AddPrivateCaster(Caster *);
			void AddPublicCaster(Caster *);

			DeclaredObjectDefinition *GetObjectDefinition(const std::string &, bool, bool);
			Unit *GetGlobal(const std::string &, bool, bool);
			Callable *GetCallable(const std::string &, bool, bool);
			ACaster *GetCaster(const VariableType &, const VariableType &, bool, bool);
			ACaster *GetCaster(const VariableType &, const VariableType &, bool, bool, bool);
			bool HaveObjectDefinition(const std::string &, bool, bool);
			bool HaveGlobal(const std::string &, bool, bool);
			bool HaveCallable(const std::string &, bool, bool);
			bool HaveCaster(const VariableType &, const VariableType &, bool, bool);
			bool HaveCaster(const VariableType &, const VariableType &, bool, bool, bool);
			template <typename t_SearchType>
			t_SearchType *Get(const std::string &name, bool searchInPrivate, bool searchSelf, bool searchNative, t_SearchType *(Native::*nativePtr)(const std::string &), t_SearchType *(Script::*ptr)(const std::string &, bool, bool), ScriptInnerCall lock, const std::unordered_map<std::string, t_SearchType *> &privateMap, const std::unordered_map<std::string, t_SearchType *> &publicMap)
			{
				TreeLocker locker(&m_TreeLock, lock);
				if (!locker.Lock())
					return nullptr;
				if (searchNative)
				{
					auto *value = (m_Native->*nativePtr)(name);
					if (value)
						return value;
				}
				if (searchSelf)
				{
					if (searchInPrivate)
					{
						auto privateMapIt = privateMap.find(name);
						if (privateMapIt != privateMap.end())
							return (*privateMapIt).second;
					}
					auto publicMapIt = publicMap.find(name);
					if (publicMapIt != publicMap.end())
						return (*publicMapIt).second;
				}
				if (searchInPrivate)
				{
					for (Script *privateImports : m_AllImports)
					{
						auto *value = (privateImports->*ptr)(name, false, false);
						if (value)
							return value;
					}
				}
				else
				{
					for (Script *publicImport : m_Imports)
					{
						auto *value = (publicImport->*ptr)(name, false, false);
						if (value)
							return value;
					}
				}
				return nullptr;
			}
			template <typename t_SearchType>
			bool Have(const std::string &name, bool searchInPrivate, bool searchSelf, bool searchNative, bool (Native::*nativePtr)(const std::string &), bool(Script::*ptr)(const std::string &, bool, bool), ScriptInnerCall lock, const std::unordered_map<std::string, t_SearchType *> &privateMap, const std::unordered_map<std::string, t_SearchType *> &publicMap)
			{
				TreeLocker locker(&m_TreeLock, lock);
				if (!locker.Lock())
					return false;
				if ((searchNative && (m_Native->*nativePtr)(name)) || (searchSelf && ((searchInPrivate && privateMap.find(name) != privateMap.end()) || publicMap.find(name) != publicMap.end())))
					return true;
				if (searchInPrivate)
				{
					for (Script *privateImports : m_AllImports)
					{
						if ((privateImports->*ptr)(name, false, false))
							return true;
					}
				}
				else
				{
					for (Script *publicImport : m_Imports)
					{
						if ((publicImport->*ptr)(name, false, false))
							return true;
					}
				}
				return false;
			}
			template <typename t_SearchType>
			std::string CheckDuplicate(std::unordered_set<std::string> &alreadyDeclared, std::string(Script::*ptr)(std::unordered_set<std::string> &), ScriptInnerCall lock, const std::unordered_map<std::string, t_SearchType *> &map)
			{
				TreeLocker locker(&m_TreeLock, lock);
				if (!locker.Lock())
					return "";
				for (const auto &pair : map)
				{
					if (alreadyDeclared.find(pair.first) != alreadyDeclared.end())
						return pair.first;
					alreadyDeclared.insert(pair.first);
				}
				for (Script *import : m_Imports)
				{
					const std::string &duplicateFound = (import->*ptr)(alreadyDeclared);
					if (duplicateFound.size() != 0)
						return duplicateFound;
				}
				return "";
			}
			std::string CheckDuplicateObjectDefinition(std::unordered_set<std::string> &);
			std::string CheckDuplicateGlobal(std::unordered_set<std::string> &);
			std::string CheckDuplicateCallable(std::unordered_set<std::string> &);
			std::string CheckDuplicateCaster(std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> &);
			bool CheckType(const VariableType &);
			bool CompileImport(Script *, std::unordered_set<std::string> &, std::unordered_set<std::string> &, std::unordered_set<std::string> &, std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> &);
			bool CompileImports();
			bool CompileDeclaredObjectDefinition(DeclaredObjectDefinition *);
			bool CompileDeclaredObjectDefinitions();
			bool CompileGlobals();
			bool CompileFunction(Function *);
			bool CompileFunctions();
			bool CompileCaster(Caster *);
			bool CompileCasters();
			void Clear();
			Unit *CreateUnit(const Logger &, const std::string &, const std::string &, const std::string &, bool, const std::vector<Tag> &);
			Unit *CreateUnit(const Logger &output, const std::string &variableName, const std::string &variableTypeName, bool searchInNative, const std::vector<Tag> &tags) { return CreateUnit(output, variableName, variableTypeName, "", searchInNative, tags); }
			Unit *CreateVar(const Logger &, const std::string &, const std::string &, const std::string &, bool, const std::vector<Tag> &);
			Unit *CreateVar(const Logger &output, const std::string &variableName, const std::string &variableTypeName, bool searchInNative, const std::vector<Tag> &tags) { return CreateVar(output, variableName, variableTypeName, "", searchInNative, tags); }
			Variable CreateVar(const std::string &, const std::string &, const std::string &, bool, const std::vector<Tag> &);
			Variable CreateVar(const std::string &, const std::string &, bool, const std::vector<Tag> &);
			Variable GetGlobal(const std::string &, bool);
			bool Compile();
			const std::string &GetName() const { return m_ScriptName; }
			Native *GetNative() { return m_Native; }

		public:
			~Script();
		};
	}
}