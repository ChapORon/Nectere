#include "Dp/Node.hpp"

#include <sstream>

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

		bool Node::HaveNamedChild() const
		{
			for (const auto &child : m_Childs)
			{
				if (!child.m_Name.empty())
					return true;
			}
			return false;
		}

		void Node::InternalAdd(const std::string &key, const std::vector<Node> &values, AddType addType)
		{
			Node newNode;
			for (const Node &value : values)
			{
				Node tmp = value;
				tmp.SetName("");
				newNode.m_Childs.emplace_back(tmp);
			}
			InternalAdd(key, newNode, addType);
		}

		void Node::InternalAdd(const std::string &key, const Node &value, AddType addType)
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
							child.InternalAdd(newKey, value, addType);
							return;
						}
					}
				}
				if (nb > 0)
					return;
				m_Childs.emplace_back(Node(name));
				m_Childs[m_Childs.size() - 1].InternalAdd(newKey, value, addType);
			}
			else
			{
				std::string name = key;
				int nb = ExtractPos(name);
				unsigned int n = 0;
				for (auto &child : m_Childs)
				{
					if (child.GetName() == name)
					{
						if (nb <= 0)
						{
							switch (addType)
							{
							case Nectere::Dp::Node::AddType::INSERT:
							{
								Node newNode = value;
								newNode.SetName(name);
								m_Childs.insert(m_Childs.begin() + n, newNode);
								break;
							}
							case Nectere::Dp::Node::AddType::REPLACE:
							{
								child = value;
								break;
							}
							case Nectere::Dp::Node::AddType::MERGE:
							default:
							{
								for (const auto &valueChild : value.m_Childs)
									child.m_Childs.emplace_back(valueChild);
								break;
							}
							}
							return;
						}
						--nb;
					}
					++n;
				}
				Node newNode = value;
				newNode.SetName(name);
				m_Childs.emplace_back(newNode);
				
			}
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

		bool Node::operator==(const Node &other) const
		{
			if (m_IsNullNode && other.m_IsNullNode)
				return true;
			return m_Name == other.m_Name &&
				m_Null == other.m_Null &&
				m_Value == other.m_Value &&
				m_Childs == other.m_Childs;
		}
	}

	std::ostream &operator<<(std::ostream &os, const Dp::Node &data)
	{
		os << data.GetValue();
		return os;
	}
}