#include "Script/NativeStringCaster.hpp"

#include <string>
#include "Script/Unit.hpp"
#include "Script/VariableTypeHelper.hpp"

namespace Nectere
{
	namespace Script
	{
		bool NativeStringCaster::CanCast(const VariableType &from, const VariableType &to)
		{
			return (to == EVariableType::String && VariableTypeHelper::IsRaw(from));
		}

		bool NativeStringCaster::Cast(const Unit *from, Unit *to)
		{
			switch (from->m_Type.GetType())
			{
			case EVariableType::Void:
				break;
			case EVariableType::Bool:
			{
				to->m_StringValue = (from->m_BoolValue) ? "true" : "false";
				break;
			}
			case EVariableType::Char:
			{
				to->m_StringValue = std::to_string(from->m_CharValue);
				break;
			}
			case EVariableType::UnsignedChar:
			{
				to->m_StringValue = std::to_string(from->m_UCharValue);
				break;
			}
			case EVariableType::Short:
			{
				to->m_StringValue = std::to_string(from->m_ShortValue);
				break;
			}
			case EVariableType::UnsignedShort:
			{
				to->m_StringValue = std::to_string(from->m_UShortValue);
				break;
			}
			case EVariableType::Int:
			{
				to->m_StringValue = std::to_string(from->m_IntValue);
				break;
			}
			case EVariableType::UnsignedInt:
			{
				to->m_StringValue = std::to_string(from->m_UIntValue);
				break;
			}
			case EVariableType::Long:
			{
				to->m_StringValue = std::to_string(from->m_LongValue);
				break;
			}
			case EVariableType::UnsignedLong:
			{
				to->m_StringValue = std::to_string(from->m_ULongValue);
				break;
			}
			case EVariableType::LongLong:
			{
				to->m_StringValue = std::to_string(from->m_LongLongValue);
				break;
			}
			case EVariableType::UnsignedLongLong:
			{
				to->m_StringValue = std::to_string(from->m_ULongLongValue);
				break;
			}
			case EVariableType::Float:
			{
				to->m_StringValue = std::to_string(from->m_FloatValue);
				break;
			}
			case EVariableType::Double:
			{
				to->m_StringValue = std::to_string(from->m_DoubleValue);
				break;
			}
			case EVariableType::LongDouble:
			{
				to->m_StringValue = std::to_string(from->m_LongDoubleValue);
				break;
			}
			case EVariableType::String:
			{
				to->m_StringValue = from->m_StringValue;
			}
			case EVariableType::Array:
			case EVariableType::Group:
			case EVariableType::Object:
			case EVariableType::Count:
			case EVariableType::Invalid:
			default:
				return false;
			}
			to->m_Null = false;
			return true;
		}
	}
}