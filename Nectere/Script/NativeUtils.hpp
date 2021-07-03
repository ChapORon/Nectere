#pragma once

#include "Logger.hpp"
#include "Script/CodingStyle.hpp"
#include "Script/LinkerWrapper.hpp"
#include "Script/Native.hpp"
#include "Script/NativeFunction/DeclaredObjectFunction.hpp"
#include "Script/NativeFunction/RawFunction.hpp"
#include "Script/NativeFunction/VoidFunction.hpp"
#include "Script/SingleScriptParser.hpp"

namespace Nectere
{
	namespace Script
	{
		class NativeUtils final
		{
			friend class Interpreter;
			friend class Native;
			template <typename t_DecalredObjectType>
			friend class NativeFunction::DeclaredObjectFunction;
		private:
			std::unordered_map<size_t, std::vector<Callable *>> m_UnInitializedCallable;

		private:
			template <typename t_CallableReturnType>
			bool InternalRegisterCallable(Native *native, const std::string &signature, Callable *callable, const std::vector<Tag> &tags)
			{
				if (!callable)
				{
					LOG_SCRIPT_INIT(LogType::Error, "Cannot create new native callable: Out of memory");
					return false;
				}
				callable->AddTags(tags);
				bool isPrivate;
				if (!SingleScriptParser::ParseSignature(signature, isPrivate, callable))
				{
					delete(callable);
					return false;
				}
				if (std::is_same<void, t_CallableReturnType>::value ||
					std::is_same<bool, t_CallableReturnType>::value ||
					std::is_same<char, t_CallableReturnType>::value ||
					std::is_same<unsigned char, t_CallableReturnType>::value ||
					std::is_same<short, t_CallableReturnType>::value ||
					std::is_same<unsigned short, t_CallableReturnType>::value ||
					std::is_same<int, t_CallableReturnType>::value ||
					std::is_same<unsigned int, t_CallableReturnType>::value ||
					std::is_same<long, t_CallableReturnType>::value ||
					std::is_same<unsigned long, t_CallableReturnType>::value ||
					std::is_same<long long, t_CallableReturnType>::value ||
					std::is_same<unsigned long long, t_CallableReturnType>::value ||
					std::is_same<float, t_CallableReturnType>::value ||
					std::is_same<double, t_CallableReturnType>::value ||
					std::is_same<long double, t_CallableReturnType>::value ||
					std::is_same<std::string, t_CallableReturnType>::value ||
					std::is_same<Variable, t_CallableReturnType>::value)
					return native->AddCallable(callable);
				return native->AddCallable(typeid(t_CallableReturnType).hash_code(), callable);
			}

			template <typename t_CallableReturnType>
			bool RegisterCallable(Native *native, const std::string &signature, const t_CallableReturnType &(*function)(CallStack &), const std::vector<Tag> &tags)
			{
				Callable *newCallable = new NativeFunction::DeclaredObjectFunction(native->m_Helper, native, function);
				size_t hash = typeid(t_CallableReturnType).hash_code();
				if (m_UnInitializedCallable.find(hash) == m_UnInitializedCallable.end())
					m_UnInitializedCallable[hash] = std::vector<Callable *>();
				m_UnInitializedCallable[hash].emplace_back(newCallable);
				return InternalRegisterCallable<t_CallableReturnType>(native, signature, newCallable, tags);
			}

			template <typename t_CallableReturnType>
			bool RegisterCallable(Native *native, const std::string &signature, t_CallableReturnType &(*function)(CallStack &), const std::vector<Tag> &tags)
			{
				Callable *newCallable = new NativeFunction::DeclaredObjectFunction(native->m_Helper, native, function);
				size_t hash = typeid(t_CallableReturnType).hash_code();
				if (m_UnInitializedCallable.find(hash) == m_UnInitializedCallable.end())
					m_UnInitializedCallable[hash] = std::vector<Callable *>();
				m_UnInitializedCallable[hash].emplace_back(newCallable);
				return InternalRegisterCallable<t_CallableReturnType>(native, signature, newCallable, tags);
			}

			template <typename t_CallableReturnType>
			bool RegisterCallable(Native *native, const std::string &signature, t_CallableReturnType(*function)(CallStack &), const std::vector<Tag> &tags)
			{
				Callable *newCallable = new NativeFunction::DeclaredObjectFunction(native->m_Helper, native, function);
				size_t hash = typeid(t_CallableReturnType).hash_code();
				if (m_UnInitializedCallable.find(hash) == m_UnInitializedCallable.end())
					m_UnInitializedCallable[hash] = std::vector<Callable *>();
				m_UnInitializedCallable[hash].emplace_back(newCallable);
				return InternalRegisterCallable<t_CallableReturnType>(native, signature, newCallable, tags);
			}

			template <typename t_LinkerType>
			bool RegisterLinker(Native *native, Linker<t_LinkerType> &linker)
			{
				if (!CodingStyle::CheckName(CodingStyle::NameType::DeclaredObject, linker.m_TypeName))
					return false;
				for (const auto &subType : linker.m_subtypes)
				{
					if (!CodingStyle::CheckName(CodingStyle::NameType::Variable, subType.m_Name))
						return false;
				}
				if (native->m_RegisteredTypes.find(linker.m_TypeName) != native->m_RegisteredTypes.end())
				{
					LOG_SCRIPT_INIT(LogType::Error, "Linker for ", linker.m_TypeName, " already exist");
					return false;
				}
				if (LinkerWrapper<t_LinkerType> *wrapper = new LinkerWrapper<t_LinkerType>(linker))
				{
					if (wrapper->Compile(native))
					{
						size_t hash = typeid(t_LinkerType).hash_code();
						auto it = m_UnInitializedCallable.find(hash);
						if (it != m_UnInitializedCallable.end())
						{
							for (Callable *callable : (*it).second)
								dynamic_cast<NativeFunction::DeclaredObjectFunction<t_LinkerType> *>(callable)->SetLinkerWrapper(wrapper);
						}
						native->m_Serializers[hash] = wrapper;
						native->m_RegisteredTypes.insert(linker.m_TypeName);
						LOG_SCRIPT_INIT(LogType::Verbose, "Linker for ", linker.m_TypeName, " added");
						return true;
					}
					return false;
				}
				LOG_SCRIPT_INIT(LogType::Error, "Cannot create new wrapper for linker of ", linker.m_TypeName, ": Out of memory");
				return false;
			}

			template <typename t_TypeToConvert>
			bool AddGlobal(Native *native, const std::string &name, const std::vector<Tag> &tags, const t_TypeToConvert &value)
			{
				if (!CodingStyle::CheckName(CodingStyle::NameType::Global, name))
					return false;
				if (native->m_Globals.find(name) != native->m_Globals.end())
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "A global with the name ", name, " already exist");
					return false;
				}
				Variable newGlobal = Convert(native, name, value);
				Unit *newGlobalUnit = newGlobal.m_Unit;
				if (newGlobalUnit)
				{
					newGlobal.m_Unit = nullptr;
					newGlobalUnit->AddTags(tags);
					native->m_Globals[name] = newGlobalUnit;
					return true;
				}
				return false;
			}

			template <typename t_TypeToConvert>
			bool AddGlobal(Native *native, const std::string &name, const std::vector<Tag> &tags, const std::vector<t_TypeToConvert> &value)
			{
				if (!CodingStyle::CheckName(CodingStyle::NameType::Global, name))
					return false;
				if (native->m_Globals.find(name) != native->m_Globals.end())
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "A global with the name ", name, " already exist");
					return false;
				}
				Variable newGlobal = Convert(native, name, value);
				Unit *newGlobalUnit = newGlobal.m_Unit;
				if (newGlobalUnit)
				{
					newGlobal.m_Unit = nullptr;
					newGlobalUnit->AddTags(tags);
					native->m_Globals[name] = newGlobalUnit;
					return true;
				}
				return false;
			}

			template <typename ...t_Arguments>
			bool AddGlobal(Native *native, const std::string &name, const std::vector<Tag> &tags, t_Arguments &&...args)
			{
				if (!CodingStyle::CheckName(CodingStyle::NameType::Global, name))
					return false;
				if (native->m_Globals.find(name) != native->m_Globals.end())
				{
					LOG_SCRIPT_RUNTIME(LogType::Error, "A global with the name ", name, " already exist");
					return false;
				}
				std::vector<Variable> globalElements;
				(globalElements.emplace_back(Convert(native, std::forward<t_Arguments>(args))), ...);
				Variable newGlobal(globalElements);
				Unit *newGlobalUnit = newGlobal.m_Unit;
				if (newGlobalUnit)
				{
					newGlobal.m_Unit = nullptr;
					newGlobalUnit->AddTags(tags);
					native->m_Globals[name] = newGlobalUnit;
					return true;
				}
				return false;
			}

			template <typename t_TypeToType>
			VariableType TypeOf(Native *native)
			{
				size_t hash = typeid(t_TypeToType).hash_code();
				auto it = native->m_Serializers.find(hash);
				if (it != native->m_Serializers.end())
				{
					auto *linker = static_cast<LinkerWrapper<t_TypeToType> *>((*it).second);
					if (linker)
						return linker->GetTypeOf();
				}
				LOG_SCRIPT_RUNTIME(LogType::Error, "Unknown type: ", typeid(t_TypeToType).name());
				return VariableType();
			}

			template <typename t_TypeToConvert>
			Variable Convert(Native *native, const std::string &name, const t_TypeToConvert &value)
			{
				size_t hash = typeid(t_TypeToConvert).hash_code();
				auto it = native->m_Serializers.find(hash);
				if (it != native->m_Serializers.end())
				{
					auto *linker = static_cast<LinkerWrapper<t_TypeToConvert> *>((*it).second);
					if (linker)
					{
						LOG_SCRIPT_INIT(LogType::Verbose, "Converting declared object");
						return linker->Convert(name, value, native->m_Helper);
					}
				}
				LOG_SCRIPT_RUNTIME(LogType::Error, "Unknown type: ", typeid(t_TypeToConvert).name());
				return Variable::NULL_VARIABLE;
			}

			template <typename t_TypeToConvert>
			Variable Convert(Native *native, const std::string &name, const std::vector<t_TypeToConvert> &value)
			{
				VariableType arrayType = TypeOf<t_TypeToConvert>(native);
				std::vector<Variable> elements;
				for (auto &element : value)
				{
					Variable convertedElement = Convert(native, name, element);
					if (!convertedElement.IsNull())
						elements.emplace_back(convertedElement);
					else
						return Variable::NULL_VARIABLE;
				}
				return Variable(name, elements, arrayType);
			}

			template <typename t_TypeToFill>
			bool Fill(Native *native, const Variable &variable, t_TypeToFill &value)
			{
				size_t hash = typeid(t_TypeToFill).hash_code();
				auto it = native->m_Serializers.find(hash);
				if (it != native->m_Serializers.end())
				{
					auto *linker = static_cast<LinkerWrapper<t_TypeToFill> *>((*it).second);
					if (linker)
						return linker->Fill(variable, value);
				}
				LOG_SCRIPT_RUNTIME(LogType::Error, "Unknown type: ", typeid(t_TypeToFill).name());
				return false;
			}
		};

		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const std::string &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const bool &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const char &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const unsigned char &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const short &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const unsigned short &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const int &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const unsigned int &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const long &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const unsigned long &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const long long &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const unsigned long long &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const float &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const double &value) { return Variable(name, value); }
		template <> inline Variable NativeUtils::Convert(Native *, const std::string &name, const long double &value) { return Variable(name, value); }

		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, std::string &value) { value = static_cast<std::string>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, bool &value) { value = static_cast<bool>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, char &value) { value = static_cast<char>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, unsigned char &value) { value = static_cast<unsigned char>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, short &value) { value = static_cast<short>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, unsigned short &value) { value = static_cast<unsigned short>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, int &value) { value = static_cast<int>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, unsigned int &value) { value = static_cast<unsigned int>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, long &value) { value = static_cast<long>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, unsigned long &value) { value = static_cast<unsigned long>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, long long &value) { value = static_cast<long long>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, unsigned long long &value) { value = static_cast<unsigned long long>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, float &value) { value = static_cast<float>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, double &value) { value = static_cast<double>(variable); return true; }
		template <> inline bool NativeUtils::Fill(Native *, const Variable &variable, long double &value) { value = static_cast<long double>(variable); return true; }

		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const std::string &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<std::string>(native, signature, new NativeFunction::RawFunction<std::string>(EVariableType::String, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, std::string &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<std::string>(native, signature, new NativeFunction::RawFunction<std::string>(EVariableType::String, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, std::string(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<std::string>(native, signature, new NativeFunction::RawFunction<std::string>(EVariableType::String, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const char &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<char>(native, signature, new NativeFunction::RawFunction<char>(EVariableType::Char, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, char &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<char>(native, signature, new NativeFunction::RawFunction<char>(EVariableType::Char, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, char (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<char>(native, signature, new NativeFunction::RawFunction<char>(EVariableType::Char, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const unsigned char &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned char>(native, signature, new NativeFunction::RawFunction<unsigned char>(EVariableType::UnsignedChar, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned char &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned char>(native, signature, new NativeFunction::RawFunction<unsigned char>(EVariableType::UnsignedChar, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned char (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned char>(native, signature, new NativeFunction::RawFunction<unsigned char>(EVariableType::UnsignedChar, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const short &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<short>(native, signature, new NativeFunction::RawFunction<short>(EVariableType::Short, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, short &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<short>(native, signature, new NativeFunction::RawFunction<short>(EVariableType::Short, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, short (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<short>(native, signature, new NativeFunction::RawFunction<short>(EVariableType::Short, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const unsigned short &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned short>(native, signature, new NativeFunction::RawFunction<unsigned short>(EVariableType::UnsignedShort, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned short &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned short>(native, signature, new NativeFunction::RawFunction<unsigned short>(EVariableType::UnsignedShort, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned short (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned short>(native, signature, new NativeFunction::RawFunction<unsigned short>(EVariableType::UnsignedShort, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const int &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<int>(native, signature, new NativeFunction::RawFunction<int>(EVariableType::Int, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, int &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<int>(native, signature, new NativeFunction::RawFunction<int>(EVariableType::Int, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, int (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<int>(native, signature, new NativeFunction::RawFunction<int>(EVariableType::Int, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const unsigned int &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned int>(native, signature, new NativeFunction::RawFunction<unsigned int>(EVariableType::UnsignedInt, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned int &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned int>(native, signature, new NativeFunction::RawFunction<unsigned int>(EVariableType::UnsignedInt, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned int (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned int>(native, signature, new NativeFunction::RawFunction<unsigned int>(EVariableType::UnsignedInt, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long>(native, signature, new NativeFunction::RawFunction<long>(EVariableType::Long, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long>(native, signature, new NativeFunction::RawFunction<long>(EVariableType::Long, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long>(native, signature, new NativeFunction::RawFunction<long>(EVariableType::Long, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const unsigned long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long>(native, signature, new NativeFunction::RawFunction<unsigned long>(EVariableType::UnsignedLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long>(native, signature, new NativeFunction::RawFunction<unsigned long>(EVariableType::UnsignedLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned long (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long>(native, signature, new NativeFunction::RawFunction<unsigned long>(EVariableType::UnsignedLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const long long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long long>(native, signature, new NativeFunction::RawFunction<long long>(EVariableType::LongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long long>(native, signature, new NativeFunction::RawFunction<long long>(EVariableType::LongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long long (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long long>(native, signature, new NativeFunction::RawFunction<long long>(EVariableType::LongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const unsigned long long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long long>(native, signature, new NativeFunction::RawFunction<unsigned long long>(EVariableType::UnsignedLongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned long long &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long long>(native, signature, new NativeFunction::RawFunction<unsigned long long>(EVariableType::UnsignedLongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, unsigned long long (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<unsigned long long>(native, signature, new NativeFunction::RawFunction<unsigned long long>(EVariableType::UnsignedLongLong, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const float &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<float>(native, signature, new NativeFunction::RawFunction<float>(EVariableType::Float, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, float &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<float>(native, signature, new NativeFunction::RawFunction<float>(EVariableType::Float, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, float (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<float>(native, signature, new NativeFunction::RawFunction<float>(EVariableType::Float, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const double &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<double>(native, signature, new NativeFunction::RawFunction<double>(EVariableType::Double, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, double &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<double>(native, signature, new NativeFunction::RawFunction<double>(EVariableType::Double, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, double (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<double>(native, signature, new NativeFunction::RawFunction<double>(EVariableType::Double, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, const long double &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long double>(native, signature, new NativeFunction::RawFunction<long double>(EVariableType::LongDouble, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long double &(*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long double>(native, signature, new NativeFunction::RawFunction<long double>(EVariableType::LongDouble, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, long double (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<long double>(native, signature, new NativeFunction::RawFunction<long double>(EVariableType::LongDouble, function), tags); }
		template <> inline bool NativeUtils::RegisterCallable(Native *native, const std::string &signature, void (*function)(CallStack &), const std::vector<Tag> &tags) { return InternalRegisterCallable<void>(native, signature, new NativeFunction::VoidFunction(function), tags); }
	}
}