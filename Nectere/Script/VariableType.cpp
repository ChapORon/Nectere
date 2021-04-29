#include "Script/VariableType.hpp"

#include "Logger.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		const VariableType VariableType::invalid;

		VariableType::VariableType() : m_Type(EVariableType::Invalid), m_ArrayType(nullptr) {}

		VariableType::VariableType(const VariableType &other) : m_Type(other.m_Type), m_ArrayType(nullptr), m_Name(other.m_Name)
		{
			if (other.m_ArrayType)
			{
				m_ArrayType = new VariableType(*other.m_ArrayType);
				if (!m_ArrayType)
					LOG_SCRIPT_INIT(LogType::Error, "Cannot initialize variable type ", m_Name, ": Out of memory");
			}
		}

		VariableType::VariableType(const EVariableType &type, const VariableType &arrayType) : m_Type(EVariableType::Array), m_ArrayType(new VariableType(arrayType))
		{
			m_Name = "array[" + m_ArrayType->m_Name + "]";
			if (!m_ArrayType)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot initialize variable type ", m_Name, ": Out of memory");
		}

		VariableType::VariableType(const EVariableType &type, const EVariableType &arrayType) : m_Type(EVariableType::Array), m_ArrayType(new VariableType(arrayType))
		{
			m_Name = "array[" + m_ArrayType->m_Name + "]";
			if (!m_ArrayType)
				LOG_SCRIPT_INIT(LogType::Error, "Cannot initialize variable type ", m_Name, ": Out of memory");
		}

		VariableType::VariableType(const EVariableType &type) : m_Type(type), m_ArrayType(nullptr)
		{
			switch (type)
			{
			case EVariableType::Void:
			{
				m_Name = "void";
				break;
			}
			case EVariableType::Group:
			{
				m_Name = "group";
				break;
			}
			case EVariableType::Array:
			{
				m_Name = "array";
				break;
			}
			case EVariableType::Bool:
			{
				m_Name = "bool";
				break;
			}
			case EVariableType::Char:
			{
				m_Name = "char";
				break;
			}
			case EVariableType::UnsignedChar:
			{
				m_Name = "uchar";
				break;
			}
			case EVariableType::Short:
			{
				m_Name = "short";
				break;
			}
			case EVariableType::UnsignedShort:
			{
				m_Name = "ushort";
				break;
			}
			case EVariableType::Int:
			{
				m_Name = "int";
				break;
			}
			case EVariableType::UnsignedInt:
			{
				m_Name = "uint";
				break;
			}
			case EVariableType::Long:
			{
				m_Name = "long";
				break;
			}
			case EVariableType::UnsignedLong:
			{
				m_Name = "ulong";
				break;
			}
			case EVariableType::LongLong:
			{
				m_Name = "llong";
				break;
			}
			case EVariableType::UnsignedLongLong:
			{
				m_Name = "ullong";
				break;
			}
			case EVariableType::Float:
			{
				m_Name = "float";
				break;
			}
			case EVariableType::Double:
			{
				m_Name = "double";
				break;
			}
			case EVariableType::LongDouble:
			{
				m_Name = "ldouble";
				break;
			}
			case EVariableType::String:
			{
				m_Name = "string";
				break;
			}
			case EVariableType::Object:
			case EVariableType::Count:
			case EVariableType::Invalid:
			{
				m_Name = "";
				break;
			}
			}
		}

		VariableType::VariableType(const std::string &name) : m_ArrayType(nullptr), m_Name(name)
		{
			if (name == "void")
				m_Type = EVariableType::Void;
			else if (name == "group")
				m_Type = EVariableType::Group;
			else if (StringUtils::StartWith(name, "array"))
			{
				size_t nameSize = name.size();
				if (nameSize != 5)
				{
					if (nameSize > 7)
					{
						if (name[5] == '[' && name[nameSize - 1] == ']')
						{
							std::string arrayType = name.substr(6, nameSize - 7);
							if (arrayType.size() != 0)
							{
								m_ArrayType = new VariableType(arrayType);
								if (!m_ArrayType)
									LOG_SCRIPT_INIT(LogType::Error, "Cannot initialize variable type ", m_Name, ": Out of memory");
							}
							m_Type = EVariableType::Array;
						}
						else
							m_Type = EVariableType::Object;
					}
					else
						m_Type = EVariableType::Object;
				}
				else
					m_Type = EVariableType::Array;
			}
			else if (name == "bool")
				m_Type = EVariableType::Bool;
			else if (name == "char")
				m_Type = EVariableType::Char;
			else if (name == "uchar")
				m_Type = EVariableType::UnsignedChar;
			else if (name == "short")
				m_Type = EVariableType::Short;
			else if (name == "ushort")
				m_Type = EVariableType::UnsignedShort;
			else if (name == "int")
				m_Type = EVariableType::Int;
			else if (name == "uint")
				m_Type = EVariableType::UnsignedInt;
			else if (name == "long")
				m_Type = EVariableType::Long;
			else if (name == "ulong")
				m_Type = EVariableType::UnsignedLong;
			else if (name == "llong")
				m_Type = EVariableType::LongLong;
			else if (name == "ullong")
				m_Type = EVariableType::UnsignedLongLong;
			else if (name == "float")
				m_Type = EVariableType::Float;
			else if (name == "double")
				m_Type = EVariableType::Double;
			else if (name == "ldouble")
				m_Type = EVariableType::LongDouble;
			else if (name == "string")
				m_Type = EVariableType::String;
			else
				m_Type = EVariableType::Object;
		}

		VariableType::~VariableType()
		{
			if (m_ArrayType)
				delete(m_ArrayType);
		}

		bool VariableType::SetArrayType(const VariableType &arrayType)
		{
			if (m_ArrayType)
				delete(m_ArrayType);
			m_ArrayType = new VariableType(arrayType);
			if (!m_ArrayType)
			{
				LOG_SCRIPT_INIT(LogType::Error, "Cannot initialize variable type ", m_Name, ": Out of memory");
				return false;
			}
			return true;
		}

		std::string VariableType::GetName() const
		{
			if (m_Type != EVariableType::Array)
				return m_Name;
			else if (m_ArrayType)
				return "array[" + m_ArrayType->GetName() + "]";
			return "array";
		}

		std::size_t VariableTypeHasher::operator()(const VariableType &s) const noexcept
		{
			return std::hash<std::string>{}(s.GetName());
		}
	}
}