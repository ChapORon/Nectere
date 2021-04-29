#pragma once

#include "Script/DeclaredObjectDefinition.hpp"
#include "Script/Taggable.hpp"
#include "Script/UnitTyper/TyperArray.hpp"
#include "Script/VariableDefinition.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace UnitTyper
		{
			class ArrayTyper;
			class BoolTyper;
			class CharTyper;
			class DoubleTyper;
			class FloatTyper;
			class GroupTyper;
			class IntTyper;
			class LongDoubleTyper;
			class LongLongTyper;
			class LongTyper;
			class ObjectTyper;
			class ShortTyper;
			class StringTyper;
			class TyperArray;
			class UnsignedCharTyper;
			class UnsignedIntTyper;
			class UnsignedLongLongTyper;
			class UnsignedLongTyper;
			class UnsignedShortTyper;
			class VoidTyper;
		}
		template <typename t_NativeTypeToCastTo>
		class NativeCaster;
		class Unit final : public Taggable
		{
			friend class UnitTyper::ArrayTyper;
			friend class UnitTyper::BoolTyper;
			friend class UnitTyper::CharTyper;
			friend class UnitTyper::DoubleTyper;
			friend class UnitTyper::FloatTyper;
			friend class UnitTyper::GroupTyper;
			friend class UnitTyper::IntTyper;
			friend class UnitTyper::LongDoubleTyper;
			friend class UnitTyper::LongLongTyper;
			friend class UnitTyper::LongTyper;
			friend class UnitTyper::ObjectTyper;
			friend class UnitTyper::ShortTyper;
			friend class UnitTyper::StringTyper;
			friend class UnitTyper::TyperArray;
			friend class UnitTyper::UnsignedCharTyper;
			friend class UnitTyper::UnsignedIntTyper;
			friend class UnitTyper::UnsignedLongLongTyper;
			friend class UnitTyper::UnsignedLongTyper;
			friend class UnitTyper::UnsignedShortTyper;
			friend class UnitTyper::VoidTyper;
			friend class Caster;
			friend class CurrentEnvironment;
			friend class Native;
			template <typename t_NativeTypeToCastTo>
			friend class NativeCaster;
			friend class NativeStringCaster;
			friend class Script;
			friend class Signature;
			friend class Variable;
		private:
			static UnitTyper::TyperArray ms_Typer;

		private:
			bool m_BoolValue{ false };
			char m_CharValue{ 0 };
			unsigned char m_UCharValue{ 0 };
			short m_ShortValue{ 0 };
			unsigned short m_UShortValue{ 0 };
			int m_IntValue{ 0 };
			unsigned int m_UIntValue{ 0 };
			long m_LongValue{ 0 };
			unsigned long m_ULongValue{ 0 };
			long long m_LongLongValue{ 0 };
			unsigned long long m_ULongLongValue{ 0 };
			float m_FloatValue{ 0 };
			double m_DoubleValue{ 0 };
			long double m_LongDoubleValue{ 0 };
			std::string m_StringValue{ "" };
			std::vector<Unit *> m_GroupValue;

		private:
			bool m_Error;
			bool m_Build;
			bool m_Null;
			bool m_IsConst;
			std::string m_Name;
			VariableType m_Type;
			std::unordered_map<std::string, Unit *> m_Child;
			const DeclaredObjectDefinition *m_Definition;
			std::string m_Default;
			const CurrentEnvironment &m_Helper;

		private:
			Unit();
			Unit(Unit *);
			Unit(const std::string &, bool);
			Unit(const std::string &, char);
			Unit(const std::string &, unsigned char);
			Unit(const std::string &, short);
			Unit(const std::string &, unsigned short);
			Unit(const std::string &, int);
			Unit(const std::string &, unsigned int);
			Unit(const std::string &, long);
			Unit(const std::string &, unsigned long);
			Unit(const std::string &, long long);
			Unit(const std::string &, unsigned long long);
			Unit(const std::string &, float);
			Unit(const std::string &, double);
			Unit(const std::string &, long double);
			Unit(const std::string &, const std::string &);
			Unit(const std::string &, const std::vector<Unit *> &);
			Unit(const std::string &, const std::vector<Unit *> &, const VariableType &);
			Unit(const DeclaredObjectDefinition *, bool, const std::string &, const CurrentEnvironment &);
			Unit(const std::string &, const DeclaredObjectDefinition *, bool, const std::string &, const CurrentEnvironment &);
			Unit(const VariableType &, const std::string &, const CurrentEnvironment &);
			Unit(const DeclaredObjectDefinition *, const std::string &, const CurrentEnvironment &);
			Unit(const std::string &, const VariableType &, const std::string &, const CurrentEnvironment &);
			Unit(const std::string &, const DeclaredObjectDefinition *, const std::string &, const CurrentEnvironment &);
			void BuildType(const DeclaredObjectDefinition *);
			void Build();
			template <typename t_TypeToCompareTo>
			bool Is() const { return false; }
			const std::string &GetName() const { return m_Name; }
			std::string ToString() const;
			std::string ToRawString() const;
			bool Set(const std::string &, bool);
			bool Set(const Unit *, bool);
			bool Set(const std::string &);
			bool Set(const Unit *);
			bool Set(const std::string &, const std::string &);
			bool Set(const std::string &, Unit *);
			bool Set(size_t, Unit *);
			Unit *Get(size_t) const;
			Unit *Get(const std::string &);
			Unit *c_Get(const std::string &) const;
			bool Have(const std::string &);
			bool c_Have(const std::string &) const;
			bool IsNull() const;
			bool IsConst() const;
			void SetConstness(bool);
			inline explicit operator bool() const { return m_BoolValue; }
			inline explicit operator char() const { return m_CharValue; }
			inline explicit operator unsigned char() const { return m_UCharValue; }
			inline explicit operator short() const { return m_ShortValue; }
			inline explicit operator unsigned short() const { return m_UShortValue; }
			inline explicit operator int() const { return m_IntValue; }
			inline explicit operator unsigned int() const { return m_UIntValue; }
			inline explicit operator long() const { return m_LongValue; }
			inline explicit operator unsigned long() const { return m_ULongValue; }
			inline explicit operator long long() const { return m_LongLongValue; }
			inline explicit operator unsigned long long() const { return m_ULongLongValue; }
			inline explicit operator float() const { return m_FloatValue; }
			inline explicit operator double() const { return m_DoubleValue; }
			inline explicit operator long double() const { return m_LongDoubleValue; }
			inline explicit operator std::string() const { return m_StringValue; }
			void ClearGroup();

		public:
			~Unit();
		};

		template <> bool Unit::Is<void>() const;
		template <> bool Unit::Is<bool>() const;
		template <> bool Unit::Is<char>() const;
		template <> bool Unit::Is<unsigned char>() const;
		template <> bool Unit::Is<short>() const;
		template <> bool Unit::Is<unsigned short>() const;
		template <> bool Unit::Is<int>() const;
		template <> bool Unit::Is<unsigned int>() const;
		template <> bool Unit::Is<long>() const;
		template <> bool Unit::Is<unsigned long>() const;
		template <> bool Unit::Is<long long>() const;
		template <> bool Unit::Is<unsigned long long>() const;
		template <> bool Unit::Is<float>() const;
		template <> bool Unit::Is<double>() const;
		template <> bool Unit::Is<long double>() const;
		template <> bool Unit::Is<std::string>() const;
	}
}