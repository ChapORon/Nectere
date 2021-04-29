#include "Dp/Node.hpp"

#include <regex>
#include <sstream>
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Dp
	{
		const Node Node::null = Node(true);

		Node::Node() :
			m_IsNullNode(false),
			m_Null(false),
			m_IsString(false),
			m_IsNotAString(false) {}

		Node::Node(bool null) :
			m_IsNullNode(null),
			m_Null(false),
			m_IsString(false),
			m_IsNotAString(false) {}

		Node::Node(const char *name) :
			m_IsNullNode(false),
			m_Null(false),
			m_IsString(false),
			m_IsNotAString(false),
			m_Name(name) {}

		Node::Node(const std::string &name) :
			m_IsNullNode(false),
			m_Null(false),
			m_IsString(false),
			m_IsNotAString(false),
			m_Name(name) {}

		Node::Node(const std::string &name, const std::string &value) :
			m_IsNullNode(false),
			m_Null(false),
			m_IsString(false),
			m_IsNotAString(false),
			m_Name(name),
			m_Value(value) {}

		Node &Node::operator=(const Node &other)
		{
			m_IsNullNode = other.m_IsNullNode;
			m_Null = other.m_Null;
			m_IsString = other.m_IsString;
			m_IsNotAString = other.m_IsNotAString;
			m_Value = other.m_Value;
			m_Childs = other.m_Childs;
			return *this;
		}

		int Node::ExtractPos(std::string &key) const
		{
			unsigned long pos;
			unsigned long endPos;
			if (!StringUtils::Find(key, '(', pos))
				return -1;
			if (!StringUtils::Find(key, ')', endPos))
				return -1;
			std::string number = key.substr(static_cast<size_t>(pos) + 1, key.length() - endPos);
			key = key.substr(0, pos) + key.substr(static_cast<size_t>(endPos) + 1);
			return std::stoi(number) - 1;
		}

		void Node::ReplaceAt(const std::string &name, int pos, const std::vector<Node> &values)
		{
			for (auto &child : m_Childs)
			{
				if (child.GetName() == name)
				{
					if (pos <= 0)
					{
						child.m_Childs.clear();
						if (values.size() == 1)
							child = values[0];
						else
						{
							child.m_Value = "";
							for (const Node &value : values)
							{
								Node newNode = value;
								newNode.SetName("");
								child.m_Childs.emplace_back(newNode);
							}
						}
						return;
					}
					--pos;
				}
				if (pos == 0)
					return;
			}
		}

		size_t Node::Size() const
		{
			return m_Childs.size();
		}

		bool Node::HaveChild() const
		{
			return !m_Childs.empty();
		}

		bool Node::HaveNamedChild() const
		{
			for (const auto &child : m_Childs)
			{
				if (!child.m_Name.empty())
					return true;
			}
			return false;
		}

		void Node::InsertAt(const std::string &name, int pos, const std::vector<Node> &values)
		{
			bool found = false;
			unsigned int firstFound = 0;
			std::vector<Node> childs;
			for (unsigned int n = 0; n != m_Childs.size(); ++n)
			{
				Node child = m_Childs[n];
				if (child.GetName() == name)
				{
					if (!found)
					{
						firstFound = n;
						found = true;
					}
					if (!childs.empty() || child.m_Childs.empty() || child.HaveNamedChild())
					{
						m_Childs.erase(m_Childs.begin() + n);
						--n;
						child.SetName("");
						childs.emplace_back(child);
					}
					else
					{
						if (child.m_Childs.size() <= pos)
						{
							for (const Node &value : values)
							{
								Node newNode = value;
								newNode.SetName("");
								child.m_Childs.emplace_back(newNode);
							}
						}
						else
						{
							for (const Node &value : values)
							{
								Node newNode = value;
								newNode.SetName("");
								child.m_Childs.insert(childs.begin() + pos, newNode);
							}
						}
						return;
					}
				}
			}
			if (!childs.empty())
			{
				if (childs.size() <= pos)
				{
					for (const Node &value : values)
					{
						Node newNode = value;
						newNode.SetName("");
						childs.emplace_back(newNode);
					}
				}
				else
				{
					for (const Node &value : values)
					{
						Node newNode = value;
						newNode.SetName("");
						childs.insert(childs.begin() + pos, newNode);
					}
				}
				Node newNode(name);
				newNode.m_Childs = childs;
				m_Childs.insert(m_Childs.begin() + firstFound, newNode);
			}
			else
			{
				for (const Node &value : values)
				{
					Node newNode = value;
					newNode.SetName(name);
					m_Childs.emplace_back(newNode);
				}
			}
		}

		void Node::Add(const std::string &key, const std::vector<Node> &values, bool replace)
		{
			unsigned long pos;
			if (StringUtils::Find(key, '.', pos))
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(static_cast<size_t>(pos) + 1);
				int nb = ExtractPos(name);
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb >= -1)
							--nb;
						if (nb < 0)
						{
							child.Add(newKey, values, replace);
							return;
						}
					}
				}
				if (nb > 0)
					return;
				m_Childs.emplace_back(Node(name));
				m_Childs[m_Childs.size() - 1].Add(newKey, values, replace);
			}
			else
			{
				//Rework here
				std::string name = key;
				int nb = ExtractPos(name);
				if (replace)
					ReplaceAt(name, nb, values);
				else
					InsertAt(name, nb, values);
			}
		}

		void Node::Add(const std::string &key, const Node &node, bool replace)
		{
			std::vector<Node> nodes;
			nodes.emplace_back(node);
			Add(key, nodes, replace);
		}

		void Node::Add(const Node &node, bool replace)
		{
			std::vector<Node> nodes;
			nodes.emplace_back(node);
			Add(node.GetName(), nodes, replace);
		}

		void Node::Emplace(const Node &node)
		{
			m_Childs.emplace_back(node);
		}

		const std::string &Node::GetValue() const
		{
			return (const std::string &)m_Value;
		}

		const std::string &Node::GetName() const
		{
			return m_Name;
		}

		bool Node::Find(const std::string &key) const
		{
			return GetNode(key) != Node::null;
		}

		const Node &Node::GetNode(const std::string &key) const
		{
			unsigned long pos;
			if (StringUtils::Find(key, '.', pos))
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(static_cast<size_t>(pos) + 1);
				int nb = ExtractPos(name);
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb >= -2)
							--nb;
						if (nb < 0)
						{
							auto &ret = child.GetNode(newKey);
							if (ret != Node::null)
								return ret;
							else if (nb == -1)
								return Node::null;
						}
					}
				}
				return Node::null;
			}
			else
			{
				std::string name = key;
				int nb = ExtractPos(name);
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb >= -2)
							--nb;
						if (nb < 0)
							return child;
						if (nb == -1)
							return Node::null;
					}
				}
				return Node::null;
			}
		}

		bool Node::Remove(const std::string &key)
		{
			unsigned long pos;
			unsigned int nodeToRemove = 0;
			if (StringUtils::Find(key, '.', pos))
			{
				std::string name = key.substr(0, pos);
				std::string newKey = key.substr(static_cast<size_t>(pos) + 1);
				int nb = ExtractPos(name);
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb >= -2)
							--nb;
						if (nb < 0)
						{
							bool ret = child.Remove(newKey);
							if (ret)
								return true;
							else if (nb == -1)
								return false;
						}
					}
					++nodeToRemove;
				}
				return false;
			}
			else
			{
				std::string name = key;
				int nb = ExtractPos(name);
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb >= -2)
							--nb;
						if (nb < 0)
						{
							m_Childs.erase(m_Childs.begin() + nodeToRemove);
							return true;
						}
						if (nb == -1)
							return false;
					}
					++nodeToRemove;
				}
				return false;
			}
		}

		void Node::SetValue(const std::string &value)
		{
			m_Value = value;
		}

		bool Node::HaveValue() const
		{
			return !m_Value.empty();
		}

		bool Node::IsEmpty() const
		{
			return m_Value.empty() && m_Childs.empty();
		}

		Node::iterator Node::begin()
		{
			return m_Childs.begin();
		}

		Node::const_iterator Node::begin() const
		{
			return m_Childs.cbegin();
		}

		Node::iterator Node::end()
		{
			return m_Childs.end();
		}

		Node::const_iterator Node::end() const
		{
			return m_Childs.cend();
		}

		bool Node::operator==(const Node &other) const
		{
			if (m_IsNullNode && other.m_IsNullNode)
				return true;
			return m_Name == other.m_Name &&
				m_Null == other.m_Null &&
				m_Value == other.m_Value &&
				m_Childs == other.m_Childs;
		}

		bool Node::operator!=(const Node &other) const
		{
			return !(other == *this);
		}

		void Node::SetName(const std::string &name)
		{
			m_Name = name;
		}

		bool Node::IsNull() const
		{
			return m_Null;
		}

		void Node::SetNull(bool value)
		{
			m_Null = value;
		}

		bool Node::IsNumerical() const
		{
			return std::regex_match(m_Value, std::regex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"));
		}

		bool Node::IsBoolean() const
		{
			return m_Value == "true" || m_Value == "false";
		}

		bool Node::IsString() const
		{
			return m_IsString;
		}

		void Node::SetIsString(bool value)
		{
			m_IsString = value;
		}

		bool Node::IsNotAString() const
		{
			return m_IsNotAString;
		}

		void Node::SetIsNotAString(bool value)
		{
			m_IsNotAString = value;
		}
	}

	std::ostream &operator<<(std::ostream &os, const Dp::Node &data)
	{
		os << data.GetValue();
		return os;
	}
}