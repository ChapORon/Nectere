#pragma once

#include <ostream>
#include <regex>
#include <string>
#include <vector>
#include "Dp/nectere_dp_export.h"
#include "Dp/Serializer.hpp"

namespace Nectere
{
	namespace Dp
	{
		class NECTEREDP_EXPORT Node final
		{
		public:
			typedef std::vector<Node>::iterator iterator;
			typedef std::vector<Node>::const_iterator const_iterator;

		public:
			enum class AddType : short
			{
				INSERT,
				REPLACE,
				MERGE,
				ADD
			};

		private:
			bool m_IsNullNode;
			bool m_Null;
			bool m_IsString;
			bool m_IsNotAString;
			bool m_IsArray;
			std::string m_Name;
			std::string m_Value;
			std::vector<Node> m_Childs;

		private:
			int ExtractPos(std::string &) const;
			void InternalAdd(const std::string &, const Node &, AddType);
			void InternalAdd(const std::string &, const std::vector<Node> &, AddType);
			template <typename t_TypeToAdd>
			inline void InternalAdd(const std::string &key, const t_TypeToAdd &value, AddType type) { InternalAdd(key, Serializer<t_TypeToAdd>().ToNode(value), type); }
			template <typename t_TypeToAdd>
			void InternalAdd(const std::string &key, const std::vector<t_TypeToAdd> &values, AddType type)
			{
				std::vector<Node> datas;
				for (const t_TypeToAdd &value : values)
					datas.emplace_back(Serializer<t_TypeToAdd>().ToNode(value));
				InternalAdd(key, datas, type);
			}

		public:
			//=========================Constructor=========================
			Node();
			explicit Node(bool); //Null constructor
			explicit Node(const char *);
			explicit Node(const std::string &);
			Node(const std::string &, const std::string &);
			//=========================Copy operator=========================
			Node &operator=(const Node &);
			//=========================Comparison operator=========================
			bool operator==(const Node &) const;
			inline bool operator!=(const Node &other) const { return !(other == *this); }
			//=========================Iteration operator=========================
			inline iterator begin() { return m_Childs.begin(); }
			inline const_iterator begin() const { return m_Childs.cbegin(); }
			inline iterator end() { return m_Childs.end(); }
			inline const_iterator end() const { return m_Childs.cend(); }
			//=========================Getter/Setter=========================
			inline bool IsNullNode() const { return m_IsNullNode; }
			inline bool IsNotNullNode() const { return !m_IsNullNode; }
			inline bool IsNull() const { return m_Null; }
			inline void SetNull(bool value) { m_Null = value; }
			inline bool IsString() const { return m_IsString; }
			inline void SetIsString(bool value) { m_IsString = value; }
			inline bool IsNotAString() const { return m_IsNotAString; }
			inline void SetIsNotAString(bool value) { m_IsNotAString = value; }
			inline bool IsArray() const { return m_IsArray; }
			inline void SetIsArray(bool value) { m_IsArray = value; }
			inline bool HaveValue() const { return !m_Value.empty(); }
			inline const std::string &GetName() const { return m_Name; }
			inline void SetName(const std::string &name) { m_Name = name; }
			inline const std::string &GetValue() const { return m_Value; }
			inline void SetValue(const std::string &value) { m_Value = value; }
			//=========================Element getter=========================
			const Node &GetNode(const std::string &) const;
			template <typename t_TypeToGet>
			inline t_TypeToGet Get(const std::string &key) const { return static_cast<t_TypeToGet>(GetNode(key)); }
			template <typename t_TypeToGet>
			inline t_TypeToGet Get(const std::string &key, const t_TypeToGet &defaultValue) const
			{
				Dp::Node node = GetNode(key);
				if (node.IsNotNullNode())
					return static_cast<t_TypeToGet>(node);
				return defaultValue;
			}
			//=========================Insertion=========================
			template <typename t_TypeToAdd>
			inline void Add(const std::string &key, t_TypeToAdd &&value) { InternalAdd(key, std::forward<t_TypeToAdd>(value), AddType::ADD); }
			inline void Add(const Node &value) { InternalAdd(value.GetName(), value, AddType::ADD); }
			template <typename t_TypeToAdd>
			inline void Insert(const std::string &key, t_TypeToAdd &&value) { InternalAdd(key, std::forward<t_TypeToAdd>(value), AddType::INSERT); }
			inline void Insert(const Node &value) { InternalAdd(value.GetName(), value, AddType::INSERT); }
			template <typename t_TypeToAdd>
			inline void Replace(const std::string &key, t_TypeToAdd &&value) { InternalAdd(key, std::forward<t_TypeToAdd>(value), AddType::REPLACE); }
			inline void Replace(const Node &value) { InternalAdd(value.GetName(), value, AddType::REPLACE); }
			template <typename t_TypeToAdd>
			inline void Merge(const std::string &key, t_TypeToAdd &&value) { InternalAdd(key, std::forward<t_TypeToAdd>(value), AddType::MERGE); }
			inline void Merge(const Node &value) { InternalAdd(value.GetName(), value, AddType::MERGE); }

			inline void Emplace(const Node &value) { m_Childs.emplace_back(value); }
			//=========================Suppression=========================
			bool Remove(const std::string &);
			//=========================Analysis=========================
			bool Find(const std::string &) const;
			inline bool IsNumerical() const { return std::regex_match(m_Value, std::regex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$")); }
			inline bool IsBoolean() const { return m_Value == "true" || m_Value == "false"; }
			inline size_t Size() const { return m_Childs.size(); }
			inline bool HaveChild() const { return !m_Childs.empty(); }
			bool HaveNamedChild() const;
			inline bool IsEmpty() const { return m_Value.empty() && m_Childs.empty(); }
			//=========================Cast operator=========================
			template <typename t_TypeToCastTo>
			inline explicit operator t_TypeToCastTo() const { return Serializer<t_TypeToCastTo>().FromNode(*this); }
		};
	}

	NECTEREDP_EXPORT std::ostream &operator<<(std::ostream &, const Dp::Node &);
}
