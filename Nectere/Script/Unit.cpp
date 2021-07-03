#include "Script/Variable.hpp"

#include <sstream>
#include "Configuration.hpp"
#include "Logger.hpp"
#include "Script/CurrentEnvironment.hpp"
#include "Script/VariableType.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		UnitTyper::TyperArray Unit::ms_Typer;
		Unit::Unit() : m_Error(true), m_Build(false), m_Null(true), m_IsConst(false), m_Type(EVariableType::Invalid), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}

		//Copy Unit//
		Unit::Unit(Unit *other) :
			m_BoolValue(other->m_BoolValue),
			m_CharValue(other->m_CharValue),
			m_UCharValue(other->m_UCharValue),
			m_ShortValue(other->m_ShortValue),
			m_UShortValue(other->m_UShortValue),
			m_IntValue(other->m_IntValue),
			m_UIntValue(other->m_UIntValue),
			m_LongValue(other->m_LongValue),
			m_ULongValue(other->m_ULongValue),
			m_LongLongValue(other->m_LongLongValue),
			m_ULongLongValue(other->m_ULongLongValue),
			m_FloatValue(other->m_FloatValue),
			m_DoubleValue(other->m_DoubleValue),
			m_LongDoubleValue(other->m_LongDoubleValue),
			m_StringValue(other->m_StringValue),
			m_Error(other->m_Error),
			m_Build(other->m_Build),
			m_Null(other->m_Null),
			m_IsConst(other->m_IsConst),
			m_Name(other->m_Name),
			m_Type(other->m_Type),
			m_Definition(other->m_Definition),
			m_Default(other->m_Default),
			m_Helper(other->m_Helper)
		{
			for (const auto &pair : other->m_Child)
			{
				Unit *newUnit = new Unit(pair.second);
				if (!newUnit)
				{
					LOG_SCRIPT_INIT(LogType::Error, "Cannot copy variable: Out of memory");
					break;
				}
				m_Child[pair.first] = newUnit;
			}
			AddTags(other);
			Build();
		}

		//==================================================Build raw type variable==================================================//
		Unit::Unit(const VariableType &type, const std::string &value, const CurrentEnvironment &helper) : Unit("", type, value, helper) {}
		Unit::Unit(const std::string &name, const VariableType &type, const std::string &value, const CurrentEnvironment &helper) :
			m_Error(false),
			m_Build(true),
			m_IsConst(false),
			m_Name(name),
			m_Type(type),
			m_Definition(nullptr),
			m_Helper(helper)
		{
			if (type == EVariableType::Array || type == EVariableType::Group)
			{
				Unit *groupSize = new Unit(EVariableType::UnsignedLongLong, "0", m_Helper);
				if (!groupSize)
				{
					m_Error = true;
				}
				else
				{
					m_Child["size"] = groupSize;
					if (value.size() != 0)
						Set(value);
					else
						m_Null = true;
				}
			}
			else
			{
				if (value.size() != 0)
					Set(value);
				else
					m_Null = true;
			}
		}

		//==================================================Build declared object type variable==================================================//
		Unit::Unit(const DeclaredObjectDefinition *type, const std::string &value, const CurrentEnvironment &helper) : Unit("", type, true, value, helper) {}
		Unit::Unit(const std::string &name, const DeclaredObjectDefinition *type, const std::string &value, const CurrentEnvironment &helper) : Unit(name, type, true, value, helper) {}
		//==================================================Smart build declared object type variable==================================================//
		Unit::Unit(const DeclaredObjectDefinition *type, bool build, const std::string &value, const CurrentEnvironment &helper) : Unit("", type, build, value, helper) {}
		Unit::Unit(const std::string &name, const DeclaredObjectDefinition *type, bool build, const std::string &value, const CurrentEnvironment &helper) :
			m_Error(false),
			m_Build(false),
			m_Null(true),
			m_IsConst(false),
			m_Name(name),
			m_Type(type->m_TypeName),
			m_Definition(type),
			m_Default(value),
			m_Helper(helper)
		{
			if (build)
			{
				if (type)
				{
					BuildType(type);
					if (value.size() != 0)
						Set(value);
					else
						m_Null = true;
				}
				else
					m_Null = true;
			}
			if (type)
				AddTags(type);
		}

		Unit::Unit(const std::string &name, bool value) : m_BoolValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Bool), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, char value) : m_CharValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Char), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, unsigned char value) : m_UCharValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::UnsignedChar), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, short value) : m_ShortValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Short), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, unsigned short value) : m_UShortValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::UnsignedShort), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, int value) : m_IntValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Int), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, unsigned int value) : m_UIntValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::UnsignedInt), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, long value) : m_LongValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Long), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, unsigned long value) : m_ULongValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::UnsignedLong), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, long long value) : m_LongLongValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::LongLong), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, unsigned long long value) : m_ULongLongValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::UnsignedLongLong), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, float value) : m_FloatValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Float), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, double value) : m_DoubleValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Double), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, long double value) : m_LongDoubleValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::LongDouble), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, const std::string &value) : m_StringValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::String), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, const std::vector<Unit *> &value) : m_GroupValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Group), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}
		Unit::Unit(const std::string &name, const std::vector<Unit *> &value, const VariableType &arrayType) : m_GroupValue(value), m_Error(false), m_Build(true), m_Null(false), m_IsConst(false), m_Name(name), m_Type(EVariableType::Array, arrayType), m_Definition(nullptr), m_Helper(static_cast<Native *>(nullptr)) {}

		void Unit::BuildType(const DeclaredObjectDefinition *definition)
		{
			m_Build = true;
			for (const auto &variable : *definition)
			{
				Unit *newUnit;
				std::string variableTypeName = variable.m_Type.GetName();
				if (variable.m_Type != EVariableType::Object)
					newUnit = new Unit(variable.m_Name, variable.m_Type, variable.m_DefaultValue, m_Helper);
				else if (variableTypeName == definition->m_TypeName)
				{
					if (Nectere::Configuration::Is("ScriptEngine.DoSmartBuild", true))
						newUnit = new Unit(variable.m_Name, definition, false, variable.m_DefaultValue, m_Helper);
					else
					{
						LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create \"", variable.m_Name, "\": Recursive type without SmartBuild enabled will cause a StackOverflow");
						newUnit = new Unit();
					}
				}
				else if (auto *definitionType = definition->GetDefinitionOf(variableTypeName))
				{
					if (Nectere::Configuration::Is("ScriptEngine.DoSmartBuild", true))
						newUnit = new Unit(variable.m_Name, definitionType, false, variable.m_DefaultValue, m_Helper);
					else
						newUnit = new Unit(variable.m_Name, definitionType, true, variable.m_DefaultValue, m_Helper);
				}
				else
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create \"", variable.m_Name, "\": Unknown type");
					newUnit = new Unit();
				}
				if (!newUnit)
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot build type ", definition->m_TypeName, ": Out of memory");
					return;
				}
				newUnit->AddTags(variable.m_Tags);
				m_Child[variable.m_Name] = newUnit;
			}
		}

		bool Unit::Set(const std::string &value, bool setSubType)
		{
			if (m_Error)
				return true;
			if (m_IsConst)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot init variable ", m_Name, " with ", value, ": ", m_Name, " is const");
				return false;
			}
			Build();
			if (value.size() == 0)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot set Unit: Empty value");
				m_Null = true;
				return false;
			}
			if (value == "null")
			{
				m_Null = true;
				return true;
			}
			return ms_Typer.Set(m_Type.GetType(), this, value, setSubType, m_Helper);
		}

		bool Unit::Set(const Unit *value, bool setSubType)
		{
			if (m_Error)
				return true;
			if (!value)
				return false;
			if (m_IsConst)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot init variable ", m_Name, " with variable ", value->m_Name, ": ", m_Name, " is const");
				return false;
			}
			Build();
			if (!value)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot set Unit: Empty value");
				m_Null = true;
				return false;
			}
			if (value->m_Null)
			{
				m_Null = true;
				return true;
			}
			if (m_Type == value->m_Type)
				return ms_Typer.Set(m_Type.GetType(), this, value, setSubType, m_Helper);
			return m_Helper.Cast(value, this);
		}

		bool Unit::Set(const std::string &value)
		{
			return Set(value, true);
		}

		bool Unit::Set(const Unit *value)
		{
			if (!value)
				return false;
			return Set(value, true);
		}

		bool Unit::Set(const std::string &key, const std::string &value)
		{
			if (key.size() == 0)
				Set(value, true);
			if (m_Error)
				return true;
			if (m_IsConst)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", key, " in variable ", m_Name, ": ", m_Name, " is const");
				return false;
			}
			Build();
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end() && (*childIt).second->Set(newKey, value))
				{
					m_Null = false;
					return true;
				}
				return false;
			}
			auto childToEditIt = m_Child.find(key);
			if (childToEditIt != m_Child.end() && (*childToEditIt).second->Set(value))
			{
				m_Null = false;
				return true;
			}
			return false;
		}

		bool Unit::Set(const std::string &key, Unit *newUnit)
		{
			if (m_Error)
				return true;
			if (!newUnit)
				return false;
			if (m_IsConst)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", key, " in variable ", m_Name, ": ", m_Name, " is const");
				return false;
			}
			Build();
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end() && (*childIt).second->Set(newKey, newUnit))
				{
					m_Null = false;
					return true;
				}
				return false;
			}
			auto childToEditIt = m_Child.find(key);
			if (childToEditIt != m_Child.end() && (*childToEditIt).second->Set(newUnit))
			{
				m_Null = false;
				return true;
			}
			return false;
		}

		bool Unit::Set(size_t idx, Unit *unit)
		{
			if (m_Error)
				return true;
			if (m_IsConst)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", idx, " in array ", m_Name, ": ", m_Name, " is const");
				return false;
			}
			if (m_Type != EVariableType::Array && m_Type != EVariableType::Group)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", idx, " in array ", m_Name, ": ", m_Name, " is neither a group nor an array");
				return false;
			}
			if (m_GroupValue.size() <= idx)
			{
				for (size_t n = m_GroupValue.size(); n <= idx; ++n)
					m_GroupValue.emplace_back(nullptr);
				m_Child["size"]->m_ULongLongValue = idx;
			}
			else if (m_GroupValue[idx])
				return m_GroupValue[idx]->Set(unit);
			Unit *newUnit = nullptr;
			if (m_Type == EVariableType::Array)
			{
				if (const VariableType *arrayType = m_Type.GetArrayType())
				{
					if (*arrayType != unit->m_Type)
					{
						if (arrayType->GetType() != EVariableType::Object)
							newUnit = new Unit(*arrayType, "", m_Helper);
						else
						{
							if (auto *definitionType = m_Helper.GetObjectDefinition(arrayType->GetName()))
							{
								if (Nectere::Configuration::Is("ScriptEngine.DoSmartBuild", true))
									newUnit = new Unit(definitionType, false, "", m_Helper);
								else
									newUnit = new Unit(definitionType, true, "", m_Helper);
							}
							else
							{
								LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", idx, " in array ", m_Name, ": Unknown type ", arrayType->GetName());
								return false;
							}
						}
						if (newUnit)
						{
							if (!m_Helper.Cast(unit, newUnit))
							{
								delete(newUnit);
								LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", idx, " in array ", m_Name, ": Cannot cast ", unit->m_Type.GetName(), " into ", arrayType->GetName());
								return false;
							}
						}
					}
					else
						newUnit = new Unit(unit);
				}
				else
				{
					newUnit = new Unit(unit);
					m_Type.SetArrayType(unit->m_Type);
				}
			}
			else
				newUnit = new Unit(unit);
			if (!newUnit)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot change element ", idx, " in array ", m_Name, ": Out of memory");
				return false;
			}
			m_GroupValue[idx] = newUnit;
			return true;
		}

		bool Unit::Have(const std::string &key)
		{
			if (m_Error)
				return false;
			if (m_IsConst)
				return c_Have(key);
			Build();
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end())
					return (*childIt).second->Have(newKey);
				return false;
			}
			auto childToGetIt = m_Child.find(key);
			if (childToGetIt == m_Child.end())
				return false;
			Unit *ret = (*childToGetIt).second;
			if (!m_IsConst)
				ret->Build();
			return true;
		}

		bool Unit::c_Have(const std::string &key) const
		{
			if (m_Error)
				return false;
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end())
					return (*childIt).second->c_Have(newKey);
				return false;
			}
			auto childToGetIt = m_Child.find(key);
			return (childToGetIt != m_Child.end());
		}

		//SmartBuild
		//It will build the declared object only when using it to avoid infinite loop with recursive type
		void Unit::Build()
		{
			if (m_Type == EVariableType::Object && !m_Build)
			{
				BuildType(m_Definition);
				if (m_Default.size() != 0)
					Set(m_Default, false);
			}
		}

		Unit *Unit::Get(const std::string &key)
		{
			if (m_Error)
				return nullptr;
			if (m_IsConst)
				return c_Get(key);
			Build();
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end())
					return (*childIt).second->Get(newKey);
				return nullptr;
			}
			auto childToGetIt = m_Child.find(key);
			if (childToGetIt == m_Child.end())
				return nullptr;
			Unit *ret = (*childToGetIt).second;
			if (!m_IsConst)
				ret->Build();
			return ret;
		}

		Unit *Unit::c_Get(const std::string &key) const
		{
			if (m_Error)
				return nullptr;
			size_t pos = key.find('.');
			if (pos != std::string::npos)
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(pos + 1);
				auto childIt = m_Child.find(name);
				if (childIt != m_Child.end())
					return (*childIt).second->Get(newKey);
				return nullptr;
			}
			auto childToGetIt = m_Child.find(key);
			if (childToGetIt == m_Child.end())
				return nullptr;
			return (*childToGetIt).second;
		}

		Unit *Unit::Get(size_t idx) const
		{
			if (m_Error)
				return nullptr;
			if (m_GroupValue.size() <= idx)
				return nullptr;
			return m_GroupValue[idx];
		}

		bool Unit::IsNull() const
		{
			return m_Null;
		}

		bool Unit::IsConst() const
		{
			return m_IsConst;
		}

		void Unit::SetConstness(bool constness)
		{
			m_IsConst = constness;
		}

		std::string Unit::ToString() const
		{
			std::stringstream prefix;
			if (Nectere::Configuration::Is("ScriptEngine.DisplayTag", true) && HaveTags())
				prefix << TagToString() << ' ';
			if (Nectere::Configuration::Is("ScriptEngine.DisplayName", true))
				prefix << m_Name << ": ";
			if (m_Error)
				prefix << "ERROR";
			else if (!m_Build)
				prefix << "null";
			else
				prefix << ms_Typer.ToString(m_Type.GetType(), this);
			return prefix.str();
		}

		std::string Unit::ToRawString() const
		{
			if (!m_Build)
				return "null";
			return ms_Typer.ToString(m_Type.GetType(), this);
		}

		void Unit::ClearGroup()
		{
			for (auto *elem : m_GroupValue)
			{
				if (elem)
					delete(elem);
			}
		}

		Unit::~Unit()
		{
			ClearGroup();
			for (const auto &children : m_Child)
				delete(children.second);
		}

		template <> bool Unit::Is<void>() const { return m_Type == EVariableType::Void; }
		template <> bool Unit::Is<bool>() const { return m_Type == EVariableType::Bool; }
		template <> bool Unit::Is<char>() const { return m_Type == EVariableType::Char; }
		template <> bool Unit::Is<unsigned char>() const { return m_Type == EVariableType::UnsignedChar; }
		template <> bool Unit::Is<short>() const { return m_Type == EVariableType::Short; }
		template <> bool Unit::Is<unsigned short>() const { return m_Type == EVariableType::UnsignedShort; }
		template <> bool Unit::Is<int>() const { return m_Type == EVariableType::Int; }
		template <> bool Unit::Is<unsigned int>() const { return m_Type == EVariableType::UnsignedInt; }
		template <> bool Unit::Is<long>() const { return m_Type == EVariableType::Long; }
		template <> bool Unit::Is<unsigned long>() const { return m_Type == EVariableType::UnsignedLong; }
		template <> bool Unit::Is<long long>() const { return m_Type == EVariableType::LongLong; }
		template <> bool Unit::Is<unsigned long long>() const { return m_Type == EVariableType::UnsignedLongLong; }
		template <> bool Unit::Is<float>() const { return m_Type == EVariableType::Float; }
		template <> bool Unit::Is<double>() const { return m_Type == EVariableType::Double; }
		template <> bool Unit::Is<long double>() const { return m_Type == EVariableType::LongDouble; }
		template <> bool Unit::Is<std::string>() const { return m_Type == EVariableType::String; }
	}
}