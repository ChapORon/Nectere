#pragma once

#include "Script/Unit.hpp"

namespace Nectere
{
	namespace Script
	{
		class Variable final
		{
			friend class CallStack;
			friend class CurrentEnvironment;
			friend class Function;
			friend class Interpreter;
			template <typename t_LinkerType>
			friend class Linker;
			friend class LocalScope;
			friend class Native;
			friend class NativeUtils;
			friend class Parameter;
			friend class Script;
			friend class Signature;
		public:
			static Variable NULL_VARIABLE;

		private:
			Unit *m_Unit;
			bool m_Delete;
			bool m_Raw;

		private:
			Variable(Unit *);
			Variable(Unit *, bool);
			Variable(Unit *, bool, bool);
			Variable(DeclaredObjectDefinition *, const CurrentEnvironment &);
			inline bool HaveUnit() { return m_Unit != nullptr; }
			Variable Get(const std::string &) const;
			Variable Get(const std::string &);
			Variable GetMutable() const;
			Variable GetCopy() const;
			template <typename t_TypeToCastTo>
			explicit operator t_TypeToCastTo() const
			{
				throw std::exception();
			}

		public:
			Variable();
			Variable(const VariableType &, const CurrentEnvironment &);
			Variable(const std::string &, bool);
			Variable(const std::string &, char);
			Variable(const std::string &, unsigned char);
			Variable(const std::string &, short);
			Variable(const std::string &, unsigned short);
			Variable(const std::string &, int);
			Variable(const std::string &, unsigned int);
			Variable(const std::string &, long);
			Variable(const std::string &, unsigned long);
			Variable(const std::string &, long long);
			Variable(const std::string &, unsigned long long);
			Variable(const std::string &, float);
			Variable(const std::string &, double);
			Variable(const std::string &, long double);
			Variable(const std::string &, std::vector<Variable> &);
			Variable(const std::string &, std::vector<Variable> &, const VariableType &);
			Variable(const std::string &, const std::string &);
			Variable(Variable &);
			Variable &operator=(Variable &);
			Variable(Variable &&) noexcept;
			Variable &operator=(Variable &&) noexcept;
			Variable(const Variable &) = delete;
			Variable &operator=(const Variable &) = delete;
			bool Set(const std::string &);
			bool Set(const std::string &, const std::string &);
			bool Set(const Variable &);
			bool Set(const std::string &, const Variable &);
			inline const VariableType &GetVariableType() const { return (m_Unit) ? m_Unit->m_Type : VariableType::invalid; }
			bool Have(const std::string &);
			bool Have(const std::string &) const;
			bool IsNull() const;
			bool IsConst() const;
			void SetConstness(bool);
			bool HaveTags() const;
			bool HaveTagWithName(const Tag &tag) const;
			bool HaveTagWithName(const std::string &tagName) const;
			bool HaveTagWithParam(const Tag &tag) const;
			bool HaveTagWithParam(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const;
			bool HaveTag(const Tag &tag) const;
			bool HaveTag(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const;
			const Tag &GetTag(const Tag &tag) const { return m_Unit->GetTag(tag.m_Name); }
			const Tag &GetTag(const std::string &tagName) const { return m_Unit->GetTag(tagName); }
			bool AddTag(const std::string &name);
			bool AddTag(const Tag &);
			bool AddTags(const std::vector<std::string> &);
			bool AddTags(const std::vector<Tag> &);
			bool AddTags(const Taggable *);
			bool RemoveTag(const Tag &tag);
			bool RemoveTag(const std::string &);
			const std::string &GetName() const;
			std::string TagToString() const;
			std::vector<Tag> GetTagList() const;
			explicit operator std::string() const
			{
				if (!m_Unit)
					return "";
				return (std::string)(*m_Unit);
			}
			explicit operator bool() const
			{
				if (!m_Unit)
					return false;
				return (bool)(*m_Unit);
			}
			explicit operator short() const
			{
				if (!m_Unit)
					return 0;
				return (short)(*m_Unit);
			}
			explicit operator unsigned short() const
			{
				if (!m_Unit)
					return 0;
				return (unsigned short)(*m_Unit);
			}
			explicit operator char() const
			{
				if (!m_Unit)
					return 0;
				return (char)(*m_Unit);
			}
			explicit operator unsigned char() const
			{
				if (!m_Unit)
					return 0;
				return (unsigned char)(*m_Unit);
			}
			explicit operator int() const
			{
				if (!m_Unit)
					return 0;
				return (int)(*m_Unit);
			}
			explicit operator unsigned int() const
			{
				if (!m_Unit)
					return 0;
				return (unsigned int)(*m_Unit);
			}
			explicit operator long() const
			{
				if (!m_Unit)
					return 0;
				return (long)(*m_Unit);
			}
			explicit operator unsigned long() const
			{
				if (!m_Unit)
					return 0;
				return (unsigned long)(*m_Unit);
			}
			explicit operator long long() const
			{
				if (!m_Unit)
					return 0;
				return (long long)(*m_Unit);
			}
			explicit operator unsigned long long() const
			{
				if (!m_Unit)
					return 0;
				return (unsigned long long)(*m_Unit);
			}
			explicit operator float() const
			{
				if (!m_Unit)
					return 0;
				return (float)(*m_Unit);
			}
			explicit operator double() const
			{
				if (!m_Unit)
					return 0;
				return (double)(*m_Unit);
			}
			explicit operator long double() const
			{
				if (!m_Unit)
					return 0;
				return (long double)(*m_Unit);
			}
			template <typename t_TypeToCompareTo>
			bool Is() const
			{
				if (!m_Unit)
					return false;
				return m_Unit->Is<t_TypeToCompareTo>();
			}
			std::string ToString() const;
			std::string ToRawString() const;
			~Variable();
		};

		std::ostream &operator<<(std::ostream &, const Variable &);
	}
}