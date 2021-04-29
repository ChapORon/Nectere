#pragma once

#include <string>
#include "Script/ACaster.hpp"
#include "Script/Unit.hpp"
#include "Script/VariableTypeHelper.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_NativeTypeToCastTo>
		class NativeCaster : public ACaster
		{
			using super = ACaster;
			friend class Native;
			friend class Script;
		private:
			VariableType m_Type;
			t_NativeTypeToCastTo Unit::*m_Value;
			t_NativeTypeToCastTo(*m_StringConverter)(const std::string &);

		private:
			bool CanCast(const VariableType &from, const VariableType &to) override
			{
				return (to == m_Type && VariableTypeHelper::IsRaw(from));
			}

			bool Cast(const Unit *from, Unit *to) override
			{
				switch (from->m_Type.GetType())
				{
				case EVariableType::Void:
					break;
				case EVariableType::Bool:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_BoolValue);
					break;
				}
				case EVariableType::Char:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_CharValue);
					break;
				}
				case EVariableType::UnsignedChar:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_UCharValue);
					break;
				}
				case EVariableType::Short:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_ShortValue);
					break;
				}
				case EVariableType::UnsignedShort:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_UShortValue);
					break;
				}
				case EVariableType::Int:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_IntValue);
					break;
				}
				case EVariableType::UnsignedInt:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_UIntValue);
					break;
				}
				case EVariableType::Long:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_LongValue);
					break;
				}
				case EVariableType::UnsignedLong:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_ULongValue);
					break;
				}
				case EVariableType::LongLong:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_LongLongValue);
					break;
				}
				case EVariableType::UnsignedLongLong:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_ULongLongValue);
					break;
				}
				case EVariableType::Float:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_FloatValue);
					break;
				}
				case EVariableType::Double:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_DoubleValue);
					break;
				}
				case EVariableType::LongDouble:
				{
					(to->*m_Value) = static_cast<t_NativeTypeToCastTo>(from->m_LongDoubleValue);
					break;
				}
				case EVariableType::String:
				{
					try
					{
						(to->*m_Value) = m_StringConverter(from->m_StringValue);
					}
					catch (const std::exception &)
					{
						to->m_Null = true;
						LOG_SCRIPT_RUNTIME(LogType::Error, "Bad format value \"", from->m_StringValue, "\": cannot cast");
						return false;
					}
				}
				case EVariableType::Array:
				case EVariableType::Group:
				case EVariableType::Object:
				case EVariableType::Count:
				case EVariableType::Invalid:
				default:
					return false;
				}
				to->m_Null = from->m_Null;
				return true;
			}

		public:
			NativeCaster(const EVariableType &type, t_NativeTypeToCastTo Unit::*value, t_NativeTypeToCastTo(*stringConverter)(const std::string &)) : m_Type(type), m_Value(value), m_StringConverter(stringConverter) {}
			virtual ~NativeCaster() = default;
		};
	}
}