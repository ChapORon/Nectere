#pragma once

#include <ostream>
#include <string>
#include <vector>
#include "Dp/Serializer.hpp"

namespace Nectere
{
	namespace Dp
	{
		class Node final
		{
		public:
			static const Node null;
			typedef std::vector<Node>::iterator iterator;
			typedef std::vector<Node>::const_iterator const_iterator;

		private:
			bool m_IsNullNode;
			bool m_Null;
			bool m_IsString;
			bool m_IsNotAString;
			std::string m_Name;
			std::string m_Value;
			std::vector<Node> m_Childs;

		private:
			explicit Node(bool);
			int ExtractPos(std::string &) const;
			void ReplaceAt(const std::string &, int, const std::vector<Node> &);
			void InsertAt(const std::string &, int, const std::vector<Node> &);
			template <typename T>
			void AddNativeType(const std::string &key, const std::vector<T> &values, bool replace = false)
			{
				std::vector<Node> datas;
				for (const T &value : values)
					datas.emplace_back(Node("", std::to_string(value)));
				Add(key, datas, replace);
			}

		public:
			//=========================Constructor=========================
			Node();
			explicit Node(const char *);
			explicit Node(const std::string &);
			Node(const std::string &, const std::string &);
			//=========================Copy operator=========================
			Node &operator=(const Node &);
			//=========================Comparison operator=========================
			bool operator==(const Node &) const;
			bool operator!=(const Node &) const;
			//=========================Iteration operator=========================
			iterator begin();
			const_iterator begin() const;
			iterator end();
			const_iterator end() const;
			//=========================Getter/Setter=========================
			bool IsNull() const;
			void SetNull(bool);
			bool IsString() const;
			void SetIsString(bool);
			bool IsNotAString() const;
			void SetIsNotAString(bool);
			bool HaveValue() const;
			const std::string &GetName() const;
			void SetName(const std::string &);
			const std::string &GetValue() const;
			void SetValue(const std::string &);
			//=========================Element getter=========================
			const Node &GetNode(const std::string &) const;
			template <typename T>
			T Get(const std::string &key) const { return Serializer<T>().FromNode(GetNode(key)); }
			template <> bool Get<bool>(const std::string &key) const { return (bool)GetNode(key); }
			template <> char Get<char>(const std::string &key) const { return (char)GetNode(key); }
			template <> int Get<int>(const std::string &key) const { return (int)GetNode(key); }
			template <> float Get<float>(const std::string &key) const { return (float)GetNode(key); }
			template <> double Get<double>(const std::string &key) const { return (double)GetNode(key); }
			template <> long Get<long>(const std::string &key) const { return (long)GetNode(key); }
			template <> std::string Get<std::string>(const std::string &key) const { return (std::string)GetNode(key); }
			template <> const std::string &Get<const std::string &>(const std::string &key) const { return (const std::string &)GetNode(key); }
			//=========================Insertion=========================
			template <typename T>
			void Add(const std::string &key, const T &value, bool replace = false)
			{
				std::vector<T> values;
				values.emplace_back(value);
				Add(key, values, replace);
			}
			template <typename T>
			void Add(const std::string &key, const std::vector<T> &values, bool replace = false)
			{
				std::vector<Node> datas;
				for (const T &value : values)
					datas.emplace_back(Serializer<T>().ToNode(value));
				Add(key, datas, replace);
			}
			template <> void Add<bool>(const std::string &key, const std::vector<bool> &values, bool replace)
			{
				std::vector<Node> datas;
				for (bool value : values)
					datas.emplace_back(Node("", value ? "true" : "false"));
				Add(key, datas, replace);
			}
			template <> void Add<char>(const std::string &key, const std::vector<char> &values, bool replace) { AddNativeType(key, values, replace); }
			template <> void Add<int>(const std::string &key, const std::vector<int> &values, bool replace) { AddNativeType(key, values, replace); }
			template <> void Add<float>(const std::string &key, const std::vector<float> &values, bool replace) { AddNativeType(key, values, replace); }
			template <> void Add<double>(const std::string &key, const std::vector<double> &values, bool replace) { AddNativeType(key, values, replace); }
			template <> void Add<long>(const std::string &key, const std::vector<long> &values, bool replace) { AddNativeType(key, values, replace); }
			template <> void Add<char *>(const std::string &key, const std::vector<char *> &values, bool replace)
			{
				std::vector<Node> datas;
				for (const char *value : values)
					datas.emplace_back(Node("", std::string(value)));
				Add(key, datas, replace);
			}
			template <> void Add<std::string>(const std::string &key, const std::vector<std::string> &values, bool replace)
			{
				std::vector<Node> datas;
				for (const std::string &value : values)
					datas.emplace_back(Node("", value));
				Add(key, datas, replace);
			}
			void Add(const Node &, bool = false);
			void Add(const std::string &, const Node &, bool = false);
			void Add(const std::string &, const std::vector<Node> &, bool);
			void Emplace(const Node &);
			//=========================Suppression=========================
			bool Remove(const std::string &);
			//=========================Analysis=========================
			bool Find(const std::string &) const;
			bool IsNumerical() const;
			bool IsBoolean() const;
			size_t Size() const;
			bool HaveChild() const;
			bool HaveNamedChild() const;
			bool IsEmpty() const;
			//=========================Cast operator=========================
			explicit operator const std::string &() const
			{
				return m_Value;
			}
			explicit operator std::string() const
			{
				return m_Value;
			}
			explicit operator bool() const
			{
				return (m_Value == "true");
			}
			explicit operator char() const
			{
				return m_Value[0];
			}
			explicit operator int() const
			{
				return std::stoi(m_Value);
			}
			explicit operator float() const
			{
				return std::stof(m_Value);
			}
			explicit operator double() const
			{
				return std::stod(m_Value);
			}
			explicit operator long() const
			{
				return std::stol(m_Value);
			}
		};
	}

	std::ostream &operator<<(std::ostream &, const Dp::Node &);
}
