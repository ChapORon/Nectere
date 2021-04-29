#pragma once

#include <sstream>
#include "Script/ObjectLinker.hpp"
#include "Script/PrimitiveLinker.hpp"
#include "Script/StringLinker.hpp"
#include "Script/Native.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_LinkerType>
		class LinkerWrapper;
		template <typename t_LinkerType>
		class Linker final : public Taggable
		{
			template <typename t_OtherLinkerType>
			friend class Linker;
			friend class LinkerWrapper<t_LinkerType>;
			friend class Native;
			friend class NativeUtils;
			template <typename t_LinkerObjectType, typename t_ObjectSubType>
			friend class ObjectLinker;
		private:
			Native *m_Native;
			bool m_Compiled;
			std::string m_TypeName;
			VariableType m_TypeOf;
			std::unordered_set<std::string> m_AddedVariables;
			std::vector<VariableDefinition> m_Subtypes;
			std::unordered_map<std::string, SubTypeLinker<t_LinkerType> *> m_SubTypeLinkers;
			DeclaredObjectDefinition *m_Definiton;

		private:
			VariableType GetTypeOf() { return m_TypeOf; }

			bool Compile(Native *native)
			{
				m_Native = native;
				m_Definiton = new DeclaredObjectDefinition();
				if (!m_Definiton)
				{
					LOG_SCRIPT_COMPILATION(LogType::CRASH, "Cannot create new object definition for linker of ", m_TypeName, ": Out of memory");
					return false;
				}
				m_Definiton->AddTags(this);
				m_Definiton->m_typeName = m_TypeName;
				for (const auto &variableDefinition : m_Subtypes)
				{
					std::string definitionType = variableDefinition.m_type.GetName();
					if (variableDefinition.m_type == EVariableType::OBJECT)
					{
						DeclaredObjectDefinition *subTypeDefinition = native->GetObjectDefinition(definitionType);
						if (subTypeDefinition)
						{
							m_Definiton->AddDefinitionOf(subTypeDefinition);
						}
						else
						{
							LOG_SCRIPT_COMPILATION(LogType::CRASH, "Cannot compile type \"", m_TypeName, "\": Unknown type ", definitionType);
							delete(m_Definiton);
							return false;
						}
					}
					if (!m_Definiton->AddVariable(variableDefinition.m_name, variableDefinition.m_type, variableDefinition.m_defaultValue, variableDefinition.m_tags))
					{
						delete(m_Definiton);
						return false;
					}
				}
				if (!native->AddObjectDefinition(m_Definiton))
				{
					LOG_SCRIPT_COMPILATION(LogType::CRASH, "Cannot add definition of linker to interpreter: ", m_TypeName, " already exist");
					delete(m_Definiton);
					return false;
				}
				m_Compiled = true;
				return true;
			}

			std::string ToString(const t_LinkerType &value) const
			{
				std::stringstream stream;
				bool first = true;
				stream << '{';
				for (const auto &variableDefinition : m_Subtypes)
				{
					if (first)
						first = false;
					else
						stream << ", ";
					std::string subValueName = variableDefinition.m_name;
					auto it = m_SubTypeLinkers.find(subValueName);
					if (it == m_SubTypeLinkers.end())
					{
						LOG_SCRIPT_INIT(LogType::CRASH, "Unknown sub value ", subValueName);
						return "";
					}
					stream << (*it).second->ToString(value);
				}
				stream << '}';
				return stream.str();
			}

			Variable Convert(const std::string &name, const t_LinkerType &value, const CurrentEnvironment &helper) const
			{
				if (!m_Compiled)
				{
					LOG_SCRIPT_RUNTIME(LogType::CRASH, "Linker is not compiled yet, please register it to an Interpreter");
					return Variable::NULL_VARIABLE;
				}
				Variable variable(m_Definiton, helper);
				if (!variable.HaveUnit())
					return Variable::NULL_VARIABLE;
				for (const auto &variableDefinition : m_Subtypes)
				{
					std::string subValueName = variableDefinition.m_name;
					auto it = m_SubTypeLinkers.find(subValueName);
					if (it == m_SubTypeLinkers.end())
					{
						LOG_SCRIPT_RUNTIME(LogType::CRASH, "Unknown sub value ", subValueName);
						return Variable::NULL_VARIABLE;
					}
					Variable valueOfSubType = (*it).second->Convert(subValueName, value, helper);
					if (!valueOfSubType.HaveUnit())
						return Variable::NULL_VARIABLE;
					if (!variable.Set(subValueName, valueOfSubType))
						return Variable::NULL_VARIABLE;
				}
				return variable;
			}

			bool Fill(const Variable &variable, t_LinkerType &dest) const
			{
				if (!m_Compiled)
				{
					LOG_SCRIPT_RUNTIME(LogType::CRASH, "Linker is not compiled yet, please register it to an Interpreter");
					return false;
				}
				for (const auto &variableDefinition : m_Subtypes)
				{
					std::string subValueName = variableDefinition.m_name;
					Variable valueOfSubType = variable.Get(subValueName);
					auto it = m_SubTypeLinkers.find(subValueName);
					if (!valueOfSubType.HaveUnit())
						return false;
					else if (it == m_SubTypeLinkers.end())
					{
						LOG_SCRIPT_RUNTIME(LogType::CRASH, "Unknown sub value ", subValueName);
						return false;
					}
					else if (!(*it).second->Fill(dest, valueOfSubType))
						return false;
				}
				return true;
			}

			bool RegisterLinker(const std::string &name, const VariableType &variableType, SubTypeLinker<t_LinkerType> *subTypeLinker, const std::string &defaultValue, const std::vector<Tag> &tags)
			{
				if (!subTypeLinker)
				{
					LOG_SCRIPT_INIT(LogType::CRASH, "Cannot create new linker for sub type ", variableType.GetName(), " for linker of ", m_TypeName, ": Out of memory");
					return false;
				}
				if (!(*subTypeLinker))
				{
					LOG_SCRIPT_INIT(LogType::CRASH, "Invalid parameters while registering getter and setter");
					delete(subTypeLinker);
					return false;
				}
				if (m_AddedVariables.find(name) != m_AddedVariables.end())
				{
					LOG_SCRIPT_INIT(LogType::CRASH, "Variable ", name, " already exist within the object");
					delete(subTypeLinker);
					return false;
				}
				m_AddedVariables.insert(name);
				m_Subtypes.emplace_back(VariableDefinition{ name, variableType, defaultValue, tags });
				m_SubTypeLinkers[name] = subTypeLinker;
				return true;
			}

			template <typename t_Primitive>
			bool RegisterPrimitiveGetterSetter(const std::string &name, const EVariableType &type, t_Primitive(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(t_Primitive), const std::string &defaultValue) { return RegisterLinker(name, VariableType(type), new PrimitiveLinker<t_LinkerType, t_Primitive>(getter, setter), defaultValue, std::vector<Tag>()); }
			template <typename t_Primitive>
			bool RegisterPrimitiveGetterSetter(const std::string &name, const EVariableType &type, const std::vector<Tag> &tags, t_Primitive(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(t_Primitive), const std::string &defaultValue) { return RegisterLinker(name, VariableType(type), new PrimitiveLinker<t_LinkerType, t_Primitive>(getter, setter), defaultValue, tags); }

			template <typename t_Primitive>
			bool RegisterPrimitiveAccess(const std::string &name, const EVariableType &type, t_Primitive t_LinkerType::*rawAccess, const std::string &defaultValue) { return RegisterLinker(name, VariableType(type), new PrimitiveLinker<t_LinkerType, t_Primitive>(rawAccess), defaultValue, std::vector<Tag>()); }
			template <typename t_Primitive>
			bool RegisterPrimitiveAccess(const std::string &name, const EVariableType &type, const std::vector<Tag> &tags, t_Primitive t_LinkerType::*rawAccess, const std::string &defaultValue) { return RegisterLinker(name, VariableType(type), new PrimitiveLinker<t_LinkerType, t_Primitive>(rawAccess), defaultValue, tags); }

		public:
			Linker(const std::string &typeName) : m_Native(nullptr), m_Compiled(false), m_TypeName(typeName), m_TypeOf(typeName), m_Definiton(nullptr) {}
			Linker(Linker<t_LinkerType> &other) :
				m_Native(other.m_native),
				m_Compiled(other.m_compiled),
				m_TypeName(other.m_typeName),
				m_TypeOf(other.m_typeOf),
				m_AddedVariables(other.m_addedVariables),
				m_Subtypes(other.m_subtypes),
				m_SubTypeLinkers(other.m_subTypeLinkers),
				m_Definiton(other.m_definiton)
			{
				other.m_subTypeLinkers.clear();
				AddTags(&other);
			}
			Linker &operator=(Linker<t_LinkerType> &other)
			{
				m_Native = other.m_native;
				m_Compiled = other.m_compiled;
				m_TypeName = other.m_typeName;
				m_TypeOf = other.m_typeOf;
				m_AddedVariables = other.m_addedVariables;
				m_Subtypes = other.m_subtypes;
				m_SubTypeLinkers = other.m_subTypeLinkers;
				m_Definiton = other.m_definiton;
				other.m_subTypeLinkers.clear();
				return *this;
			}
			Linker(const Linker<t_LinkerType> &) = delete;
			Linker &operator=(const Linker<t_LinkerType> &) = delete;
			//object
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, t_ReturnType &(t_LinkerType::*getterSetter)() const, const Linker<t_ReturnType> &otherLinker) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(getterSetter, otherLinker), "", std::vector<Tag>()); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, t_ReturnType t_LinkerType::*rawAccess, const Linker<t_ReturnType> &otherLinker) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(rawAccess, otherLinker), "", std::vector<Tag>()); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, t_ReturnType &(t_LinkerType::*getterSetter)() const, const Linker<t_ReturnType> &otherLinker, const t_ReturnType &defaultValue) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(getterSetter, otherLinker), otherLinker.ToString(defaultValue), std::vector<Tag>()); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, t_ReturnType t_LinkerType::*rawAccess, const Linker<t_ReturnType> &otherLinker, const t_ReturnType &defaultValue) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(rawAccess, otherLinker), otherLinker.ToString(defaultValue), std::vector<Tag>()); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, t_ReturnType &(t_LinkerType::*getterSetter)() const, const Linker<t_ReturnType> &otherLinker) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(getterSetter, otherLinker), "", tags); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, t_ReturnType t_LinkerType::*rawAccess, const Linker<t_ReturnType> &otherLinker) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(rawAccess, otherLinker), "", tags); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, t_ReturnType &(t_LinkerType::*getterSetter)() const, const Linker<t_ReturnType> &otherLinker, const t_ReturnType &defaultValue) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(getterSetter, otherLinker), otherLinker.ToString(defaultValue), tags); }
			template <typename t_ReturnType>
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, t_ReturnType t_LinkerType::*rawAccess, const Linker<t_ReturnType> &otherLinker, const t_ReturnType &defaultValue) { return RegisterLinker(name, VariableType(otherLinker.m_typeName), new ObjectLinker<t_LinkerType, t_ReturnType>(rawAccess, otherLinker), otherLinker.ToString(defaultValue), tags); }
			//string
			inline bool Register(const std::string &name, const std::string &(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(const std::string &)) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(getter, setter), "", std::vector<Tag>()); }
			inline bool Register(const std::string &name, std::string t_LinkerType::*rawAccess) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(rawAccess), "", std::vector<Tag>()); }
			inline bool Register(const std::string &name, const std::string &(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(const std::string &), const std::string &defaultValue) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(getter, setter), defaultValue, std::vector<Tag>()); }
			inline bool Register(const std::string &name, std::string t_LinkerType::*rawAccess, const std::string &defaultValue) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(rawAccess), defaultValue, std::vector<Tag>()); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, const std::string &(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(const std::string &)) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(getter, setter), "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, std::string t_LinkerType::*rawAccess) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(rawAccess), "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, const std::string &(t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(const std::string &), const std::string &defaultValue) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(getter, setter), defaultValue, tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, std::string t_LinkerType::*rawAccess, const std::string &defaultValue) { return RegisterLinker(name, VariableType(EVariableType::STRING), new StringLinker<t_LinkerType>(rawAccess), defaultValue, tags); }
			//bool
			inline bool Register(const std::string &name, bool (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(bool)) { return RegisterPrimitiveGetterSetter(name, EVariableType::BOOL, getter, setter, ""); }
			inline bool Register(const std::string &name, bool t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::BOOL, rawAccess, ""); }
			inline bool Register(const std::string &name, bool (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(bool), bool defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::BOOL, getter, setter, (defaultValue) ? "true" : "false"); }
			inline bool Register(const std::string &name, bool t_LinkerType::*rawAccess, bool defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::BOOL, rawAccess, (defaultValue) ? "true" : "false"); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, bool (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(bool)) { return RegisterPrimitiveGetterSetter(name, EVariableType::BOOL, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, bool t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::BOOL, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, bool (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(bool), bool defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::BOOL, getter, setter, (defaultValue) ? "true" : "false", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, bool t_LinkerType::*rawAccess, bool defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::BOOL, rawAccess, (defaultValue) ? "true" : "false", tags); }
			//char
			inline bool Register(const std::string &name, char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(char)) { return RegisterPrimitiveGetterSetter(name, EVariableType::CHAR, getter, setter, ""); }
			inline bool Register(const std::string &name, char t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::CHAR, rawAccess, ""); }
			inline bool Register(const std::string &name, char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(char), char defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::CHAR, getter, setter, std::to_string(static_cast<int>(defaultValue))); }
			inline bool Register(const std::string &name, char t_LinkerType::*rawAccess, char defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::CHAR, rawAccess, std::to_string(static_cast<int>(defaultValue))); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(char)) { return RegisterPrimitiveGetterSetter(name, EVariableType::CHAR, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, char t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::CHAR, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(char), char defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::CHAR, getter, setter, std::to_string(static_cast<int>(defaultValue)), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, char t_LinkerType::*rawAccess, char defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::CHAR, rawAccess, std::to_string(static_cast<int>(defaultValue)), tags); }
			//unsigned char
			inline bool Register(const std::string &name, unsigned char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned char)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_CHAR, getter, setter, ""); }
			inline bool Register(const std::string &name, unsigned char t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_CHAR, rawAccess, ""); }
			inline bool Register(const std::string &name, unsigned char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned char), unsigned char defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_CHAR, getter, setter, std::to_string(static_cast<int>(defaultValue))); }
			inline bool Register(const std::string &name, unsigned char t_LinkerType::*rawAccess, unsigned char defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_CHAR, rawAccess, std::to_string(static_cast<int>(defaultValue))); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned char)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_CHAR, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned char t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_CHAR, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned char (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned char), unsigned char defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_CHAR, getter, setter, std::to_string(static_cast<int>(defaultValue)), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned char t_LinkerType::*rawAccess, unsigned char defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_CHAR, rawAccess, std::to_string(static_cast<int>(defaultValue)), tags); }
			//short
			inline bool Register(const std::string &name, short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(short)) { return RegisterPrimitiveGetterSetter(name, EVariableType::SHORT, getter, setter, ""); }
			inline bool Register(const std::string &name, short t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::SHORT, rawAccess, ""); }
			inline bool Register(const std::string &name, short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(short), short defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::SHORT, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, short t_LinkerType::*rawAccess, short defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::SHORT, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(short)) { return RegisterPrimitiveGetterSetter(name, EVariableType::SHORT, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, short t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::SHORT, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(short), short defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::SHORT, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, short t_LinkerType::*rawAccess, short defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::SHORT, rawAccess, std::to_string(defaultValue), tags); }
			//unsigned short
			inline bool Register(const std::string &name, unsigned short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned short)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_SHORT, getter, setter, ""); }
			inline bool Register(const std::string &name, unsigned short t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_SHORT, rawAccess, ""); }
			inline bool Register(const std::string &name, unsigned short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned short), unsigned short defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_SHORT, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, unsigned short t_LinkerType::*rawAccess, unsigned short defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_SHORT, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned short)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_SHORT, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned short t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_SHORT, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned short (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned short), unsigned short defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_SHORT, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned short t_LinkerType::*rawAccess, unsigned short defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_SHORT, rawAccess, std::to_string(defaultValue), tags); }
			//int
			inline bool Register(const std::string &name, int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(int)) { return RegisterPrimitiveGetterSetter(name, EVariableType::INT, getter, setter, ""); }
			inline bool Register(const std::string &name, int t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::INT, rawAccess, ""); }
			inline bool Register(const std::string &name, int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(int), int defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::INT, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, int t_LinkerType::*rawAccess, int defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::INT, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(int)) { return RegisterPrimitiveGetterSetter(name, EVariableType::INT, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, int t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::INT, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(int), int defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::INT, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, int t_LinkerType::*rawAccess, int defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::INT, rawAccess, std::to_string(defaultValue), tags); }
			//unsigned int
			inline bool Register(const std::string &name, unsigned int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned int)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_INT, getter, setter, ""); }
			inline bool Register(const std::string &name, unsigned int t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_INT, rawAccess, ""); }
			inline bool Register(const std::string &name, unsigned int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned int), unsigned int defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_INT, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, unsigned int t_LinkerType::*rawAccess, unsigned int defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_INT, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned int)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_INT, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned int t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_INT, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned int (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned int), unsigned int defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_INT, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned int t_LinkerType::*rawAccess, unsigned int defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_INT, rawAccess, std::to_string(defaultValue), tags); }
			//long
			inline bool Register(const std::string &name, long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG, getter, setter, ""); }
			inline bool Register(const std::string &name, long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG, rawAccess, ""); }
			inline bool Register(const std::string &name, long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long), long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, long t_LinkerType::*rawAccess, long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long), long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long t_LinkerType::*rawAccess, long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG, rawAccess, std::to_string(defaultValue), tags); }
			//unsigned long
			inline bool Register(const std::string &name, unsigned long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG, getter, setter, ""); }
			inline bool Register(const std::string &name, unsigned long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG, rawAccess, ""); }
			inline bool Register(const std::string &name, unsigned long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long), unsigned long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, unsigned long t_LinkerType::*rawAccess, unsigned long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long), unsigned long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long t_LinkerType::*rawAccess, unsigned long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG, rawAccess, std::to_string(defaultValue), tags); }
			//long long
			inline bool Register(const std::string &name, long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_LONG, getter, setter, ""); }
			inline bool Register(const std::string &name, long long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG_LONG, rawAccess, ""); }
			inline bool Register(const std::string &name, long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long long), long long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_LONG, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, long long t_LinkerType::*rawAccess, long long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG_LONG, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_LONG, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG_LONG, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long long), long long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_LONG, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long long t_LinkerType::*rawAccess, long long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG_LONG, rawAccess, std::to_string(defaultValue), tags); }
			//unsigned long long
			inline bool Register(const std::string &name, unsigned long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG_LONG, getter, setter, ""); }
			inline bool Register(const std::string &name, unsigned long long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG_LONG, rawAccess, ""); }
			inline bool Register(const std::string &name, unsigned long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long long), unsigned long long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG_LONG, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, unsigned long long t_LinkerType::*rawAccess, unsigned long long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG_LONG, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long long)) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG_LONG, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long long t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG_LONG, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long long (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(unsigned long long), unsigned long long defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::UNSIGNED_LONG_LONG, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, unsigned long long t_LinkerType::*rawAccess, unsigned long long defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::UNSIGNED_LONG_LONG, rawAccess, std::to_string(defaultValue), tags); }
			//float
			inline bool Register(const std::string &name, float (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(float)) { return RegisterPrimitiveGetterSetter(name, EVariableType::FLOAT, getter, setter, ""); }
			inline bool Register(const std::string &name, float t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::FLOAT, rawAccess, ""); }
			inline bool Register(const std::string &name, float (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(float), float defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::FLOAT, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, float t_LinkerType::*rawAccess, float defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::FLOAT, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, float (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(float)) { return RegisterPrimitiveGetterSetter(name, EVariableType::FLOAT, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, float t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::FLOAT, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, float (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(float), float defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::FLOAT, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, float t_LinkerType::*rawAccess, float defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::FLOAT, rawAccess, std::to_string(defaultValue), tags); }
			//double
			inline bool Register(const std::string &name, double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(double)) { return RegisterPrimitiveGetterSetter(name, EVariableType::DOUBLE, getter, setter, ""); }
			inline bool Register(const std::string &name, double t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::DOUBLE, rawAccess, ""); }
			inline bool Register(const std::string &name, double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(double), double defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::DOUBLE, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, double t_LinkerType::*rawAccess, double defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::DOUBLE, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(double)) { return RegisterPrimitiveGetterSetter(name, EVariableType::DOUBLE, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, double t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::DOUBLE, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(double), double defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::DOUBLE, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, double t_LinkerType::*rawAccess, double defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::DOUBLE, rawAccess, std::to_string(defaultValue), tags); }
			//long double
			inline bool Register(const std::string &name, long double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long double)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_DOUBLE, getter, setter, ""); }
			inline bool Register(const std::string &name, long double t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG_DOUBLE, rawAccess, ""); }
			inline bool Register(const std::string &name, long double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long double), long double defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_DOUBLE, getter, setter, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, long double t_LinkerType::*rawAccess, long double defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG_DOUBLE, rawAccess, std::to_string(defaultValue)); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long double)) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_DOUBLE, getter, setter, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long double t_LinkerType::*rawAccess) { return RegisterPrimitiveAccess(name, EVariableType::LONG_DOUBLE, rawAccess, "", tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long double (t_LinkerType::*getter)() const, void (t_LinkerType::*setter)(long double), long double defaultValue) { return RegisterPrimitiveGetterSetter(name, EVariableType::LONG_DOUBLE, getter, setter, std::to_string(defaultValue), tags); }
			inline bool Register(const std::string &name, const std::vector<Tag> &tags, long double t_LinkerType::*rawAccess, long double defaultValue) { return RegisterPrimitiveAccess(name, EVariableType::LONG_DOUBLE, rawAccess, std::to_string(defaultValue), tags); }

			~Linker()
			{
				for (auto &pair : m_SubTypeLinkers)
					delete(pair.second);
			}
		};
	}
}