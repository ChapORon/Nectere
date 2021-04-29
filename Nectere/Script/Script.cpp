#include "Script/Script.hpp"

#include "Script/Caster.hpp"
#include "Script/CodingStyle.hpp"
#include "Script/CurrentEnvironment.hpp"
#include "Script/DeclaredObjectDefinition.hpp"
#include "Script/Function.hpp"
#include "Script/ScriptLoader.hpp"
#include "Script/Variable.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		Script::TreeLock::TreeLock() : m_Locks()
		{
			for (int n = 0; n != SCRIPT_INNER_CALL_SIZE; ++n)
				m_Locks[n] = false;
		}

		bool Script::TreeLock::Lock(ScriptInnerCall call)
		{
			if (m_Locks[static_cast<int>(call)])
				return false;
			m_Locks[static_cast<int>(call)] = true;
			return true;
		}

		void Script::TreeLock::Unlock(ScriptInnerCall call)
		{
			m_Locks[static_cast<int>(call)] = false;
		}

		Script::Script(Native *native, const std::string &scriptName) : m_Helper(this), m_ScriptName(scriptName), m_Compiled(false), m_Native(native) {}

		DeclaredObjectDefinition *Script::GetObjectDefinition(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Get<DeclaredObjectDefinition>(name, searchInPrivate, true, searchInNative, &Native::GetObjectDefinition, &Script::GetObjectDefinition, ScriptInnerCall::GetObjectDefinition, m_PrivateDeclaredObjectDefinition, m_DeclaredObjectDefinition);
		}

		bool Script::HaveObjectDefinition(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Have<DeclaredObjectDefinition>(name, searchInPrivate, true, searchInNative, &Native::HaveObjectDefinition, &Script::HaveObjectDefinition, ScriptInnerCall::HaveObjectDefinition, m_PrivateDeclaredObjectDefinition, m_DeclaredObjectDefinition);
		}

		Unit *Script::GetGlobal(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Get<Unit>(name, searchInPrivate, true, searchInNative, &Native::GetGlobal, &Script::GetGlobal, ScriptInnerCall::GetGlobal, m_PrivateGlobals, m_Globals);
		}

		bool Script::HaveGlobal(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Have<Unit>(name, searchInPrivate, true, searchInNative, &Native::HaveGlobal, &Script::HaveGlobal, ScriptInnerCall::HaveGlobal, m_PrivateGlobals, m_Globals);
		}

		Callable *Script::GetCallable(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Get<Callable>(name, searchInPrivate, true, searchInNative, &Native::GetCallable, &Script::GetCallable, ScriptInnerCall::GetCallable, m_PrivateFunctions, m_Functions);
		}

		bool Script::HaveCallable(const std::string &name, bool searchInPrivate, bool searchInNative)
		{
			return Have<Callable>(name, searchInPrivate, true, searchInNative, &Native::HaveCallable, &Script::HaveCallable, ScriptInnerCall::HaveCallable, m_PrivateFunctions, m_Functions);
		}

		ACaster *Script::GetCaster(const VariableType &from, const VariableType &to, bool searchInPrivate, bool searchNative)
		{
			return GetCaster(from, to, searchInPrivate, true, searchNative);
		}

		ACaster *Script::GetCaster(const VariableType &from, const VariableType &to, bool searchInPrivate, bool searchSelf, bool searchNative)
		{
			TreeLocker locker(&m_TreeLock, ScriptInnerCall::GetCaster);
			if (!locker.Lock())
				return nullptr;
			if (searchNative)
			{
				auto *value = m_Native->GetCaster(from, to);
				if (value)
					return value;
			}
			if (searchSelf)
			{
				if (searchInPrivate)
				{
					auto privateMapIt = m_PrivateCasters.find(from);
					if (privateMapIt != m_PrivateCasters.end())
					{
						std::vector<Caster *> casters = (*privateMapIt).second;
						for (Caster *caster : casters)
						{
							if (caster->CanCast(from, to))
								return caster;
						}
					}
				}
				auto publicMapIt = m_Casters.find(from);
				if (publicMapIt != m_Casters.end())
				{
					std::vector<Caster *> casters = (*publicMapIt).second;
					for (Caster *caster : casters)
					{
						if (caster->CanCast(from, to))
							return caster;
					}
				}
			}
			if (searchInPrivate)
			{
				for (Script *privateImports : m_AllImports)
				{
					auto *value = privateImports->GetCaster(from, to, false, false);
					if (value)
						return value;
				}
			}
			else
			{
				for (Script *publicImport : m_Imports)
				{
					auto *value = publicImport->GetCaster(from, to, false, false);
					if (value)
						return value;
				}
			}
			return nullptr;
		}

		bool Script::HaveCaster(const VariableType &from, const VariableType &to, bool searchInPrivate, bool searchNative)
		{
			return HaveCaster(from, to, searchInPrivate, true, searchNative);
		}

		bool Script::HaveCaster(const VariableType &from, const VariableType &to, bool searchInPrivate, bool searchSelf, bool searchNative)
		{
			TreeLocker locker(&m_TreeLock, ScriptInnerCall::HaveCaster);
			if (!locker.Lock())
				return false;
			if (searchNative && m_Native->CanCast(from, to))
				return true;
			if (searchSelf)
			{
				if (searchInPrivate)
				{
					auto privateMapIt = m_PrivateCasters.find(from);
					if (privateMapIt != m_PrivateCasters.end())
					{
						std::vector<Caster *> casters = (*privateMapIt).second;
						for (Caster *caster : casters)
						{
							if (caster->CanCast(from, to))
								return true;
						}
					}
				}
				auto publicMapIt = m_Casters.find(from);
				if (publicMapIt != m_Casters.end())
				{
					std::vector<Caster *> casters = (*publicMapIt).second;
					for (Caster *caster : casters)
					{
						if (caster->CanCast(from, to))
							return true;
					}
				}
			}
			if (searchInPrivate)
			{
				for (Script *privateImports : m_AllImports)
				{
					if (privateImports->HaveCaster(from, to, false, false))
						return true;
				}
			}
			else
			{
				for (Script *publicImport : m_Imports)
				{
					if (publicImport->HaveCaster(from, to, false, false))
						return true;
				}
			}
			return false;
		}

		Variable Script::GetGlobal(const std::string &name, bool searchInNative)
		{
			if (!m_Compiled)
				return Variable::NULL_VARIABLE;
			return Variable(GetGlobal(name, true, searchInNative), false);
		}

		Unit *Script::CreateUnit(const Logger &output, const std::string &variableName, const std::string &variableTypeName, const std::string &variableValue, bool searchInNative, const std::vector<Tag> &tags)
		{
			Unit *newVariable;
			VariableType variableType = VariableType(variableTypeName);
			if (variableType == EVariableType::Object)
			{
				DeclaredObjectDefinition *variableTypeDefinition = GetObjectDefinition(variableTypeName, true, searchInNative);
				if (variableTypeDefinition)
				{
					output.Log(LogType::Verbose, "Creating object ", variableName, " of type ", variableTypeName);
					if (variableName.size() == 0)
						newVariable = new Unit(variableTypeDefinition, "", m_Helper);
					else
						newVariable = new Unit(variableName, variableTypeDefinition, "", m_Helper);
				}
				else
				{
					output.Log(LogType::Error, "In ", m_ScriptName, ": No definition for type \"", variableTypeName, '"');
					return nullptr;
				}
			}
			else
			{
				output.Log(LogType::Verbose, "Creating ", variableName, " of type ", variableType.GetName());
				if (variableName.size() == 0)
					newVariable = new Unit(variableType, "", m_Helper);
				else
					newVariable = new Unit(variableName, variableType, "", m_Helper);
			}
			if (!newVariable)
			{
				output.Log(LogType::Error, "Cannot create new variable: Out of memory");
				return nullptr;
			}
			if (variableValue.size() != 0 && !newVariable->Set(variableValue))
			{
				delete(newVariable);
				return nullptr;
			}
			newVariable->AddTags(tags);
			return newVariable;
		}

		Unit *Script::CreateVar(const Logger &output, const std::string &variableName, const std::string &variableTypeName, const std::string &variableValue, bool searchInNative, const std::vector<Tag> &tags)
		{
			if (!CodingStyle::CheckName(CodingStyle::NameType::Variable, variableName))
				return nullptr;
			return CreateUnit(output, variableName, variableTypeName, variableValue, searchInNative, tags);
		}

		Variable Script::CreateVar(const std::string &variableName, const std::string &variableType, const std::string &variableValue, bool searchInNative, const std::vector<Tag> &tags)
		{
			if (!m_Compiled)
				return Variable::NULL_VARIABLE;
			Unit *newUnit = CreateVar(Nectere::Logger::ScriptEngine::runtime, variableName, variableType, variableValue, searchInNative, tags);
			if (!newUnit)
				return Variable::NULL_VARIABLE;
			return Variable(newUnit, true);
		}

		Variable Script::CreateVar(const std::string &variableName, const std::string &variableType, bool searchIntNative, const std::vector<Tag> &tags)
		{
			return CreateVar(variableName, variableType, "", searchIntNative, tags);
		}

		void Script::Clear()
		{
			m_PrivateImports.clear();
			m_Imports.clear();
			for (auto &pair : m_PrivateDeclaredObjectDefinition)
				delete(pair.second);
			m_PrivateDeclaredObjectDefinition.clear();
			for (auto &pair : m_DeclaredObjectDefinition)
				delete(pair.second);
			m_DeclaredObjectDefinition.clear();
			for (auto &pair : m_PrivateFunctions)
				delete(pair.second);
			m_PrivateFunctions.clear();
			for (auto &pair : m_Functions)
				delete(pair.second);
			m_Functions.clear();
			for (auto &pair : m_PrivateGlobals)
				delete(pair.second);
			m_PrivateGlobals.clear();
			for (auto &pair : m_Globals)
				delete(pair.second);
			m_Globals.clear();
			for (auto &pair : m_PrivateCasters)
			{
				for (auto *caster : pair.second)
					delete(caster);
			}
			m_PrivateCasters.clear();
			for (auto &pair : m_Casters)
			{
				for (auto *caster : pair.second)
					delete(caster);
			}
			m_Casters.clear();
		}

		Script::~Script() { Clear(); }

		//==================================================PARSER ACCESS==================================================//
		void Script::AddGlobalToCompile(const GlobalDefinition &globalToCompile)
		{
			m_GlobalToCompile.emplace_back(globalToCompile);
		}

		void Script::AddPrivateImport(Script *importToAdd)
		{
			m_PrivateImports.emplace_back(importToAdd);
			m_AllImports.emplace_back(importToAdd);
		}

		void Script::AddPublicImport(Script *importToAdd)
		{
			m_Imports.emplace_back(importToAdd);
			m_AllImports.emplace_back(importToAdd);
		}

		void Script::AddPrivateDeclaredObjectDefinition(DeclaredObjectDefinition *declaredObjectDefinition)
		{
			m_PrivateDeclaredObjectDefinition[declaredObjectDefinition->m_TypeName] = declaredObjectDefinition;
		}

		void Script::AddPublicDeclaredObjectDefinition(DeclaredObjectDefinition *declaredObjectDefinition)
		{
			m_DeclaredObjectDefinition[declaredObjectDefinition->m_TypeName] = declaredObjectDefinition;
		}

		void Script::AddPrivateFunction(Function *newFunction)
		{
			m_PrivateFunctions[newFunction->GetName()] = newFunction;
		}

		void Script::AddPublicFunction(Function *newFunction)
		{
			m_Functions[newFunction->GetName()] = newFunction;
		}

		void Script::AddPrivateCaster(Caster *newCaster)
		{
			auto aToBIt = m_PrivateCasters.find(newCaster->m_TypeA);
			if (aToBIt == m_PrivateCasters.end())
			{
				m_PrivateCasters[newCaster->m_TypeA] = std::vector<Caster *>();
				aToBIt = m_PrivateCasters.find(newCaster->m_TypeA);
			}
			(*aToBIt).second.emplace_back(newCaster);
			if (newCaster->m_IsBidirectional)
			{
				auto bToAIt = m_PrivateCasters.find(newCaster->m_TypeB);
				if (bToAIt == m_PrivateCasters.end())
				{
					m_PrivateCasters[newCaster->m_TypeB] = std::vector<Caster *>();
					bToAIt = m_PrivateCasters.find(newCaster->m_TypeB);
				}
				(*bToAIt).second.emplace_back(newCaster);
			}
		}

		void Script::AddPublicCaster(Caster *newCaster)
		{
			auto aToBIt = m_Casters.find(newCaster->m_TypeA);
			if (aToBIt == m_Casters.end())
			{
				m_Casters[newCaster->m_TypeA] = std::vector<Caster *>();
				aToBIt = m_Casters.find(newCaster->m_TypeA);
			}
			(*aToBIt).second.emplace_back(newCaster);
			if (newCaster->m_IsBidirectional)
			{
				auto bToAIt = m_Casters.find(newCaster->m_TypeB);
				if (bToAIt == m_Casters.end())
				{
					m_Casters[newCaster->m_TypeB] = std::vector<Caster *>();
					bToAIt = m_Casters.find(newCaster->m_TypeB);
				}
				(*bToAIt).second.emplace_back(newCaster);
			}
		}

		//==================================================COMPILATION==================================================//
		std::string Script::CheckDuplicateObjectDefinition(std::unordered_set<std::string> &alreadyDeclared)
		{
			return CheckDuplicate<DeclaredObjectDefinition>(alreadyDeclared, &Script::CheckDuplicateObjectDefinition, ScriptInnerCall::CheckDuplicateObjectDefinition, m_DeclaredObjectDefinition);
		}

		std::string Script::CheckDuplicateGlobal(std::unordered_set<std::string> &alreadyDeclared)
		{
			return CheckDuplicate<Unit>(alreadyDeclared, &Script::CheckDuplicateGlobal, ScriptInnerCall::CheckDuplicateGlobal, m_Globals);
		}

		std::string Script::CheckDuplicateCallable(std::unordered_set<std::string> &alreadyDeclared)
		{
			return CheckDuplicate<Callable>(alreadyDeclared, &Script::CheckDuplicateCallable, ScriptInnerCall::CheckDuplicateCallable, m_Functions);
		}

		std::string Script::CheckDuplicateCaster(std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> &alreadyDeclared)
		{
			TreeLocker locker(&m_TreeLock, ScriptInnerCall::CheckDuplicateCaster);
			if (!locker.Lock())
				return "";
			for (const auto &pair : m_Casters)
			{
				if (alreadyDeclared.find(pair.first) != alreadyDeclared.end())
				{
					std::unordered_set<VariableType, VariableTypeHasher> &intoSet = alreadyDeclared[pair.first];
					for (Caster *caster : pair.second)
					{
						if (caster->m_TypeA == pair.first)
						{
							if (intoSet.find(caster->m_TypeB) != intoSet.end())
								return pair.first.GetName() + " to " + caster->m_TypeB.GetName();
							intoSet.insert(caster->m_TypeB);
						}
						else
						{
							if (intoSet.find(caster->m_TypeA) != intoSet.end())
								return pair.first.GetName() + " to " + caster->m_TypeA.GetName();
							intoSet.insert(caster->m_TypeA);
						}
					}
				}
				else
				{
					alreadyDeclared[pair.first] = std::unordered_set<VariableType, VariableTypeHasher>();
					std::unordered_set<VariableType, VariableTypeHasher> &intoSet = alreadyDeclared[pair.first];
					for (Caster *caster : pair.second)
					{
						if (caster->m_TypeA == pair.first)
							intoSet.insert(caster->m_TypeB);
						else
							intoSet.insert(caster->m_TypeA);
					}
				}
			}
			for (Script *import : m_Imports)
			{
				const std::string &duplicateFound = import->CheckDuplicateCaster(alreadyDeclared);
				if (duplicateFound.size() != 0)
					return duplicateFound;
			}
			return "";
		}

		bool Script::CompileImport(Script *import, std::unordered_set<std::string> &declaredObjectSet, std::unordered_set<std::string> &globalSet, std::unordered_set<std::string> &functionSet, std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> &casterSet)
		{
			if (!import->Compile())
				return false;
			LOG_SCRIPT_COMPILATION(LogType::Verbose, "Checking duplicate in private import ", import->m_ScriptName);
			const std::string &duplicateDeclaredObjectFound = import->CheckDuplicateObjectDefinition(globalSet);
			if (duplicateDeclaredObjectFound.size() != 0)
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Duplicate declared object \"", duplicateDeclaredObjectFound, "\" from import ", import->m_ScriptName);
				return false;
			}
			const std::string &duplicateGlobalFound = import->CheckDuplicateGlobal(globalSet);
			if (duplicateGlobalFound.size() != 0)
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Duplicate global \"", duplicateGlobalFound, "\" from import ", import->m_ScriptName);
				return false;
			}
			const std::string &duplicateFunctionFound = import->CheckDuplicateObjectDefinition(functionSet);
			if (duplicateFunctionFound.size() != 0)
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Duplicate function \"", duplicateFunctionFound, "\" from import ", import->m_ScriptName);
				return false;
			}
			const std::string &duplicateCasterFound = import->CheckDuplicateCaster(casterSet);
			if (duplicateFunctionFound.size() != 0)
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Duplicate caster \"", duplicateCasterFound, "\" from import ", import->m_ScriptName);
				return false;
			}
			return true;
		}

		bool Script::CompileImports()
		{
			bool ret = true;
			std::unordered_set<std::string> declaredObjectSet, globalSet, functionSet;
			std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> casterSet;
			for (auto it = m_AllImports.begin(); it != m_AllImports.end();)
			{
				std::unordered_set<std::string> tmpDeclaredObjectSet = declaredObjectSet;
				std::unordered_set<std::string> tmpGlobalSet = globalSet;
				std::unordered_set<std::string> tmpFunctionSet = functionSet;
				std::unordered_map<VariableType, std::unordered_set<VariableType, VariableTypeHasher>, VariableTypeHasher> tmpCasterSet = casterSet;
				if (!CompileImport(*it, declaredObjectSet, globalSet, functionSet, casterSet))
				{
					delete(*it);
					it = m_AllImports.erase(it);
					declaredObjectSet = tmpDeclaredObjectSet;
					globalSet = tmpGlobalSet;
					functionSet = tmpFunctionSet;
					casterSet = tmpCasterSet;
					ret = false;
				}
				else
					++it;
			}
			return ret;
		}

		bool Script::CompileDeclaredObjectDefinition(DeclaredObjectDefinition *declaredObject)
		{
			const std::string &typeName = declaredObject->m_TypeName;
			if (Get<DeclaredObjectDefinition>(typeName, true, false, true, &Native::GetObjectDefinition, &Script::GetObjectDefinition, ScriptInnerCall::GetObjectDefinition, m_PrivateDeclaredObjectDefinition, m_DeclaredObjectDefinition))
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Cannot compile type \"", typeName, "\": A public type with the same name already exist");
				return false;
			}
			else
			{
				const auto &subTypes = declaredObject->m_SubTypes;
				for (const auto &subType : subTypes)
				{
					DeclaredObjectDefinition *subTypeDefinition = GetObjectDefinition(subType, true, true);
					if (subTypeDefinition)
						declaredObject->AddDefinitionOf(subTypeDefinition);
					else
					{
						LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Cannot compile type \"", declaredObject->m_TypeName, "\": Unknown type ", subType);
						return false;
					}
				}
				return true;
			}
		}

		bool Script::CompileDeclaredObjectDefinitions()
		{
			bool ret = true;
			for (auto it = m_PrivateDeclaredObjectDefinition.begin(); it != m_PrivateDeclaredObjectDefinition.end();)
			{
				if (!CompileDeclaredObjectDefinition((*it).second))
				{
					delete((*it).second);
					it = m_PrivateDeclaredObjectDefinition.erase(it);
					ret = false;
				}
				else
					++it;
			}
			for (auto it = m_DeclaredObjectDefinition.begin(); it != m_DeclaredObjectDefinition.end();)
			{
				if (!CompileDeclaredObjectDefinition((*it).second))
				{
					delete((*it).second);
					it = m_DeclaredObjectDefinition.erase(it);
					ret = false;
				}
				else
					++it;
			}
			return ret;
		}

		bool Script::CompileGlobals()
		{
			for (const auto &globalToCompile : m_GlobalToCompile)
			{
				if (Get<Unit>(globalToCompile.m_Name, true, false, true, &Native::GetGlobal, &Script::GetGlobal, ScriptInnerCall::GetGlobal, m_PrivateGlobals, m_Globals))
				{
					LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Cannot compile global \"", globalToCompile.m_Name, "\": A public global with the same name already exist in imported script");
					return false;
				}
				else
				{
					Unit *newVariable = CreateUnit(Nectere::Logger::ScriptEngine::compilation, globalToCompile.m_Name, globalToCompile.m_TypeName, globalToCompile.m_Value, true, globalToCompile.m_Tags);
					if (!newVariable)
						return false;
					if (globalToCompile.m_IsPrivate)
						m_PrivateGlobals[globalToCompile.m_Name] = newVariable;
					else
						m_Globals[globalToCompile.m_Name] = newVariable;
				}
			}
			return true;
		}

		bool Script::CompileFunction(Function *function)
		{
			std::string functionName = function->GetName();
			if (Get<Callable>(functionName, true, false, true, &Native::GetCallable, &Script::GetCallable, ScriptInnerCall::GetCallable, m_PrivateFunctions, m_Functions))
			{
				LOG_SCRIPT_COMPILATION(LogType::Error, "In ", m_ScriptName, ": Cannot compile function \"", functionName, "\": A public function with the same name already exist");
				return false;
			}
			else
				return function->Compile();
		}

		bool Script::CompileFunctions()
		{
			bool ret = true;
			for (auto it = m_PrivateFunctions.begin(); it != m_PrivateFunctions.end();)
			{
				if (!CompileFunction(static_cast<Function *>((*it).second)))
				{
					delete((*it).second);
					it = m_PrivateFunctions.erase(it);
					ret = false;
				}
				else
					++it;
			}
			for (auto it = m_Functions.begin(); it != m_Functions.end();)
			{
				if (!CompileFunction(static_cast<Function *>((*it).second)))
				{
					delete((*it).second);
					it = m_Functions.erase(it);
					ret = false;
				}
				else
					++it;
			}
			return ret;
		}

		bool Script::CheckType(const VariableType &type)
		{
			if (type == EVariableType::Object)
			{
				std::string typeName = type.GetName();
				if (!HaveObjectDefinition(typeName, true, true))
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "No definition for type \"", typeName, '"');
					return false;
				}
				return true;
			}
			else if (type == EVariableType::Array)
			{
				if (const VariableType *arrayType = type.GetArrayType())
					return CheckType(*arrayType);
				return true;
			}
			return true;
		}

		bool Script::CheckType(const VariableType &type, bool output, bool searchNative)
		{
			if (type == EVariableType::Object)
			{
				std::string typeName = type.GetName();
				if (!HaveObjectDefinition(typeName, true, searchNative))
				{
					if (output)
						LOG_SCRIPT_RUNTIME(LogType::Error, "No definition for type \"", typeName, '"');
					return false;
				}
				return true;
			}
			else if (type == EVariableType::Array)
			{
				if (const VariableType *arrayType = type.GetArrayType())
					return CheckType(*arrayType, output, searchNative);
				return true;
			}
			return true;
		}

		bool Script::CheckType(const std::string &typeName, bool searchNative)
		{
			VariableType typeTocheck(typeName);
			return CheckType(typeTocheck, false, searchNative);
		}

		bool Script::CompileCaster(Caster *caster)
		{
			//#todo Check if all element used by caster are part of the definition
			return CheckType(caster->m_TypeA) && CheckType(caster->m_TypeB);
		}

		bool Script::CompileCasters()
		{
			bool ret = true;
			for (auto it = m_PrivateCasters.begin(); it != m_PrivateCasters.end();)
			{
				std::vector<Caster *> &casters = m_PrivateCasters.at((*it).first);
				for (auto casterIt = casters.begin(); casterIt != casters.end();)
				{
					if (!CompileCaster(*casterIt))
					{
						delete(*casterIt);
						casterIt = casters.erase(casterIt);
						ret = false;
					}
					else
						++casterIt;
				}
				if (!casters.size() == 0)
					it = m_PrivateCasters.erase(it);
				else
					++it;
			}
			for (auto it = m_Casters.begin(); it != m_Casters.end();)
			{
				std::vector<Caster *> &casters = m_Casters.at((*it).first);
				for (auto casterIt = casters.begin(); casterIt != casters.end();)
				{
					if (!CompileCaster(*casterIt))
					{
						delete(*casterIt);
						casterIt = casters.erase(casterIt);
						ret = false;
					}
					else
						++casterIt;
				}
				if (!casters.size() == 0)
					it = m_Casters.erase(it);
				else
					++it;
			}
			return ret;
		}

		bool Script::Compile()
		{
			TreeLocker locker(&m_TreeLock, ScriptInnerCall::Compile);
			if (!locker.Lock())
				return true;
			LOG_SCRIPT_COMPILATION(LogType::Verbose, "Compiling script ", m_ScriptName);
			if (!CompileImports() ||
				!CompileDeclaredObjectDefinitions() ||
				!CompileGlobals() ||
				!CompileCasters() ||
				!CompileFunctions())
				return false;
			m_Compiled = true;
			return true;
		}
	}
}