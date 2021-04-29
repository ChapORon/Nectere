#include "Script/NativeUtils.hpp"

#include "Script/Caster.hpp"
#include "Script/CurrentEnvironment.hpp"
#include "Script/DeclaredObjectDefinition.hpp"
#include "Script/Function.hpp"
#include "Script/NativeCaster.hpp"
#include "Script/NativeStringCaster.hpp"
#include "Script/Unit.hpp"
#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		static bool cast_string_to_bool(const std::string &str)
		{
			if (str == "true")
				return true;
			else if (str == "false")
				return false;
			throw std::invalid_argument("String is neither true or false");
		}
		static char cast_string_to_char(const std::string &str) { return static_cast<char>(std::stoi(str)); }
		static unsigned char cast_string_to_uchar(const std::string &str) { return static_cast<unsigned char>(std::stoi(str)); }
		static short cast_string_to_short(const std::string &str) { return static_cast<short>(std::stoi(str)); }
		static unsigned short cast_string_to_ushort(const std::string &str) { return static_cast<unsigned short>(std::stoi(str)); }
		static int cast_string_to_int(const std::string &str) { return std::stoi(str); }
		static unsigned int cast_string_to_uint(const std::string &str) { return static_cast<unsigned int>(std::stoi(str)); }
		static long cast_string_to_long(const std::string &str) { return std::stol(str); }
		static unsigned long cast_string_to_ulong(const std::string &str) { return std::stoul(str); }
		static long long cast_string_to_llong(const std::string &str) { return std::stoll(str); }
		static unsigned long long cast_string_to_ullong(const std::string &str) { return std::stoull(str); }
		static float cast_string_to_float(const std::string &str) { return std::stof(str); }
		static double cast_string_to_double(const std::string &str) { return std::stod(str); }
		static long double cast_string_to_ldouble(const std::string &str) { return std::stold(str); }

		Native::Native() : m_Helper(this)
		{
			m_NativeCasters[EVariableType::Bool] = new NativeCaster<bool>(EVariableType::Bool, &Unit::m_BoolValue, &cast_string_to_bool);
			m_NativeCasters[EVariableType::Char] = new NativeCaster<char>(EVariableType::Char, &Unit::m_CharValue, &cast_string_to_char);
			m_NativeCasters[EVariableType::UnsignedChar] = new NativeCaster<unsigned char>(EVariableType::UnsignedChar, &Unit::m_UCharValue, &cast_string_to_uchar);
			m_NativeCasters[EVariableType::Short] = new NativeCaster<short>(EVariableType::Short, &Unit::m_ShortValue, &cast_string_to_short);
			m_NativeCasters[EVariableType::UnsignedShort] = new NativeCaster<unsigned short>(EVariableType::UnsignedShort, &Unit::m_UShortValue, &cast_string_to_ushort);
			m_NativeCasters[EVariableType::Int] = new NativeCaster<int>(EVariableType::Int, &Unit::m_IntValue, &cast_string_to_int);
			m_NativeCasters[EVariableType::UnsignedInt] = new NativeCaster<unsigned int>(EVariableType::UnsignedInt, &Unit::m_UIntValue, &cast_string_to_uint);
			m_NativeCasters[EVariableType::Long] = new NativeCaster<long>(EVariableType::Long, &Unit::m_LongValue, &cast_string_to_long);
			m_NativeCasters[EVariableType::UnsignedLong] = new NativeCaster<unsigned long>(EVariableType::UnsignedLong, &Unit::m_ULongValue, &cast_string_to_ulong);
			m_NativeCasters[EVariableType::LongLong] = new NativeCaster<long long>(EVariableType::LongLong, &Unit::m_LongLongValue, &cast_string_to_llong);
			m_NativeCasters[EVariableType::UnsignedLongLong] = new NativeCaster<unsigned long long>(EVariableType::UnsignedLongLong, &Unit::m_ULongLongValue, &cast_string_to_ullong);
			m_NativeCasters[EVariableType::Float] = new NativeCaster<float>(EVariableType::Float, &Unit::m_FloatValue, &cast_string_to_float);
			m_NativeCasters[EVariableType::Double] = new NativeCaster<double>(EVariableType::Double, &Unit::m_DoubleValue, &cast_string_to_double);
			m_NativeCasters[EVariableType::LongDouble] = new NativeCaster<long double>(EVariableType::LongDouble, &Unit::m_LongDoubleValue, &cast_string_to_ldouble);
			m_NativeCasters[EVariableType::String] = new NativeStringCaster();
		}

		bool Native::AddObjectDefinition(DeclaredObjectDefinition *definition)
		{
			if (m_DeclaredObjectDefinition.find(definition->m_TypeName) != m_DeclaredObjectDefinition.end())
				return false;
			m_DeclaredObjectDefinition[definition->m_TypeName] = definition;
			return true;
		}

		DeclaredObjectDefinition *Native::GetObjectDefinition(const std::string &name)
		{
			auto it = m_DeclaredObjectDefinition.find(name);
			if (it != m_DeclaredObjectDefinition.end())
				return (*it).second;
			return nullptr;
		}

		bool Native::HaveObjectDefinition(const std::string &name)
		{
			if (m_DeclaredObjectDefinition.find(name) != m_DeclaredObjectDefinition.end())
				return true;
			return false;
		}

		Unit *Native::GetGlobal(const std::string &name)
		{
			auto it = m_Globals.find(name);
			if (it != m_Globals.end())
				return (*it).second;
			return nullptr;
		}

		Variable Native::GetGlobalVariable(const std::string &name)
		{
			Unit *unit = GetGlobal(name);
			if (unit)
				return Variable(unit, false);
			return Variable::NULL_VARIABLE;
		}

		bool Native::HaveGlobal(const std::string &name)
		{
			if (m_Globals.find(name) != m_Globals.end())
				return true;
			return false;
		}

		Callable *Native::GetCallable(const std::string &name)
		{
			auto functionIt = m_Functions.find(name);
			if (functionIt != m_Functions.end())
				return (*functionIt).second;
			auto uncompiledFunctionIt = m_UnCompiledFunctions.find(name);
			if (uncompiledFunctionIt != m_UnCompiledFunctions.end())
			{
				auto pair = (*uncompiledFunctionIt).second;
				if (m_Serializers.find(pair.first) != m_Serializers.end())
				{
					if (!pair.second->CheckSignature())
					{
						m_UnCompiledFunctions.erase(uncompiledFunctionIt);
						delete(pair.second);
						return nullptr;
					}
					m_Functions[(*uncompiledFunctionIt).first] = pair.second;
					m_UnCompiledFunctions.erase(uncompiledFunctionIt);
					return pair.second;
				}
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot build function ", name);
				m_UnCompiledFunctions.erase(uncompiledFunctionIt);
				delete(pair.second);
			}
			return nullptr;
		}

		bool Native::HaveCallable(const std::string &name)
		{
			if (m_Functions.find(name) != m_Functions.end())
				return true;
			else if (m_UnCompiledFunctions.find(name) != m_UnCompiledFunctions.end())
				return true;
			return false;
		}

		bool Native::CheckType(const VariableType &type, bool output)
		{
			if (type == EVariableType::Object)
			{
				std::string typeName = type.GetName();
				if (!HaveObjectDefinition(typeName))
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
					return CheckType(*arrayType, output);
				return true;
			}
			return true;
		}

		bool Native::CheckType(const std::string &typeName)
		{
			VariableType typeTocheck(typeName);
			return CheckType(typeTocheck, false);
		}

		Unit *Native::CreateUnit(const std::string &variableName, const std::string &variableTypeName, const std::string &variableValue, const std::vector<Tag> &tags)
		{
			Unit *newVariable;
			VariableType variableType = VariableType(variableTypeName);
			if (!CheckType(variableType, true))
				return nullptr;
			if (variableType == EVariableType::Object)
			{
				DeclaredObjectDefinition *variableTypeDefinition = GetObjectDefinition(variableTypeName);
				LOG_SCRIPT_RUNTIME(LogType::Verbose, "Creating object ", variableName, " of type ", variableTypeName);
				if (variableName.size() == 0)
					newVariable = new Unit(variableTypeDefinition, "", m_Helper);
				else
					newVariable = new Unit(variableName, variableTypeDefinition, "", m_Helper);
			}
			else
			{
				LOG_SCRIPT_RUNTIME(LogType::Verbose, "Creating ", variableName, " of type ", variableType.GetName());
				if (variableName.size() == 0)
					newVariable = new Unit(variableType, "", m_Helper);
				else
					newVariable = new Unit(variableName, variableType, "", m_Helper);
			}
			if (!newVariable)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create new variable of type ", variableTypeName, ": Out of memory");
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

		Variable Native::CreateVar(const std::string &variableName, const std::string &variableTypeName, const std::string &variableValue, const std::vector<Tag> &tags)
		{
			Unit *newVariable = CreateUnit(variableName, variableTypeName, variableValue, tags);
			if (!newVariable)
				return Variable::NULL_VARIABLE;
			return Variable(newVariable, true);
		}

		Native::~Native()
		{
			for (auto &pair : m_DeclaredObjectDefinition)
				delete(pair.second);
			for (auto &pair : m_Functions)
				delete(pair.second);
			for (auto &pair : m_Globals)
				delete(pair.second);
			for (auto &pair : m_Serializers)
				delete(pair.second);
			for (auto &pair : m_NativeCasters)
				delete(pair.second);
			for (auto &pair : m_Casters)
			{
				for (auto *caster : pair.second)
					delete(caster);
			}
		}

		bool Native::AddCallable(Callable *callable)
		{
			std::string functionName = callable->GetName();
			if (m_Functions.find(functionName) != m_Functions.end() || m_UnCompiledFunctions.find(functionName) != m_UnCompiledFunctions.end())
			{
				LOG_SCRIPT_INIT(LogType::Error, functionName, " already exist");
				delete(callable);
				return false;
			}
			if (!callable->Compile(m_Helper))
			{
				delete(callable);
				return false;
			}
			LOG_SCRIPT_INIT(LogType::Verbose, "Adding callable ", functionName);
			m_Functions[functionName] = callable;
			return true;
		}

		bool Native::AddCallable(size_t hash, Callable *callable)
		{
			std::string functionName = callable->GetName();
			if (m_Functions.find(functionName) != m_Functions.end() || m_UnCompiledFunctions.find(functionName) != m_UnCompiledFunctions.end())
			{
				LOG_SCRIPT_INIT(LogType::Error, functionName, " already exist");
				delete(callable);
				return false;
			}
			LOG_SCRIPT_INIT(LogType::Verbose, "Adding callable ", functionName);
			m_UnCompiledFunctions[functionName] = std::pair(hash, callable);
			return true;
		}

		bool Native::AddCaster(Caster *caster)
		{
			if (!caster)
				return false;
			VariableType typeA = caster->m_TypeA;
			VariableType typeB = caster->m_TypeB;
			if (!CheckType(typeA, true) || !CheckType(typeB, true))
				return false;
			if (m_Casters.find(typeA) == m_Casters.end())
				m_Casters[typeA] = std::vector<Caster *>();
			m_Casters.at(typeA).emplace_back(caster);
			if (caster->m_IsBidirectional)
			{
				if (m_Casters.find(typeB) == m_Casters.end())
					m_Casters[typeB] = std::vector<Caster *>();
				m_Casters.at(typeB).emplace_back(caster);
			}
			return true;
		}

		ACaster *Native::GetCaster(const VariableType &from, const VariableType &to)
		{
			auto nativeIt = m_NativeCasters.find(from.GetType());
			if (nativeIt != m_NativeCasters.end())
			{
				if (ACaster *caster = (*nativeIt).second)
				{
					if (caster->CanCast(from, to))
						return caster;
				}
			}
			auto it = m_Casters.find(from);
			if (it == m_Casters.end())
				return nullptr;
			std::vector<Caster *> casters = (*it).second;
			for (Caster *caster : casters)
			{
				if (caster->CanCast(from, to))
					return caster;
			}
			return nullptr;
		}

		bool Native::CanCast(const VariableType &from, const VariableType &to)
		{
			auto nativeIt = m_NativeCasters.find(from.GetType());
			if (nativeIt != m_NativeCasters.end())
			{
				if (ACaster *caster = (*nativeIt).second)
				{
					if (caster->CanCast(from, to))
						return true;
				}
			}
			auto it = m_Casters.find(from);
			if (it == m_Casters.end())
				return false;
			std::vector<Caster *> casters = (*it).second;
			for (Caster *caster : casters)
			{
				if (caster->CanCast(from, to))
					return true;
			}
			return false;
		}
	}
}