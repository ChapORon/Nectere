#include "Dp/Xml.hpp"

#include <algorithm>
#include <fstream>

namespace Nectere
{
	namespace Dp
	{
		const Node NULL_NODE = Node(true);

		void Str(const Node &node, Result &result);

		static bool StartWith(const std::string &str, const std::string &search, size_t pos)
		{
			for (unsigned long n = 0; n != search.length(); ++n)
			{
				if ((n + pos) > str.length() ||
					str[n + pos] != search[n])
					return false;
			}
			return true;
		}

		static bool IsWhitespace(char c)
		{
			return (c == ' ' || c == '\t' || c == '\n');
		}

		static void ByPassTrailing(const std::string &content, size_t &pos)
		{
			char current = content[pos];
			while (IsWhitespace(current))
			{
				++pos;
				current = content[pos];
			}
		}

		Node Xml::Create(const std::string &name, const std::string &version, const std::string &encoding)
		{
			Node node(name);
			node.Add("__xmldeclarations__.xml.version", version);
			node.Add("__xmldeclarations__.xml.encoding", encoding);
			return node;
		}

		template <typename t_AttributeType>
		void Xml::AddAttribute(Node &node, const std::string &key, const t_AttributeType &value)
		{
			if (StringUtils::Find(key, '.'))
				return;
			std::string attributeKey = "__xmlattributes__.";
			attributeKey += key;
			node.Add(attributeKey, value);
		}

		void Xml::AddDeclaration(Node &node, const std::string &key, const std::string &attribute, const std::string &value)
		{
			if (StringUtils::Find(key, '.') || StringUtils::Find(attribute, '.'))
				return;
			std::string attributeKey = "__xmldeclarations__.";
			attributeKey += key + '.' + attribute;
			node.Add(attributeKey, value);
		}

		static void AddValue(Result &content, const std::string &value)
		{
			for (char c : value)
			{
				switch (c)
				{
				case '<': { content.Append("&lt;"); break; }
				case '>': { content.Append("&gt;"); break; }
				case '&': { content.Append("&amp;"); break; }
				case '\'': { content.Append("&apos;"); break; }
				case '"': { content.Append("&quot;"); break; }
				case '\n':
				{
					content.Append(c);
					content.Indent();
					break;
				}
				default: { content.Append(c); break; }
				}
			}
		}

		static bool AddDeclarations(const Node &node, Result &result)
		{
			Node elements = node.GetNode("__xmldeclarations__");
			if (result.GetDepth() == 0 && elements.IsNotNullNode())
			{
				unsigned int n = 0;
				for (const auto &element : elements)
				{
					result.Indent();
					result.Append("<?");
					result.Append(element.GetName());
					for (const auto &info : element)
					{
						result.Append(' ');
						result.Append(info.GetName());
						result.Append("=\"");
						AddValue(result, info.GetValue());
						result.Append('"');
					}
					result.Append("?>");
					result.ForceLineBreak();
					++n;
				}
				return n != 0;
			}
			return false;
		}

		static bool AddAttributes(const Node &node, Result &result)
		{
			Node attributes = node.GetNode("__xmlattributes__");
			if (attributes.IsNotNullNode())
			{
				for (const auto &attribute : attributes)
				{
					result.Append(' ');
					result.Append(attribute.GetName());
					result.Append("=\"");
					AddValue(result, attribute.GetValue());
					result.Append('"');
				}
				return true;
			}
			return false;
		}

		static void AddNodes(const Node &node, Result &result)
		{
			if (node.HaveValue())
			{
				result.IncreaseDepth();
				result.Indent();
				AddValue(result, node.GetValue());
				result.DecreaseDepth();
			}
			else
			{
				unsigned int n = 0;
				for (const auto &tag : node)
				{
					if (tag.GetName().find("__") != 0)
					{
						if (n != 0)
							result.ForceLineBreak();
						result.IncreaseDepth();
						Str(tag, result);
						result.DecreaseDepth();
						++n;
					}
				}
			}
		}

		static void Str(const Node &node, Result &result)
		{
			if (result.GetDepth() == 0 && node.IsEmpty())
			{
				result.Set("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
				return;
			}
			std::string name = node.GetName();
			if (result.GetDepth() == 0 && name.empty())
				name = "root";
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (node.HaveChild() && !node.HaveNamedChild())
			{
				unsigned int n = 0;
				for (const auto &tag : node)
				{
					Node newTag = tag;
					newTag.SetName(node.GetName());
					if (n != 0)
						result.ForceLineBreak();
					Str(newTag, result);
					++n;
				}
				return;
			}
			unsigned int minSize = 0;
			if (AddDeclarations(node, result))
				++minSize;
			else if (result.GetDepth() == 0)
			{
				result.Append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
				result.ForceLineBreak();
			}
			result.Indent();
			result.Append("<" + name);
			if (AddAttributes(node, result))
				++minSize;
			if (!node.HaveValue() && node.Size() == minSize)
				result.Append(" />");
			else
			{
				result.Append('>');
				result.ForceLineBreak();
				AddNodes(node, result);
				if (!node.IsEmpty())
					result.ForceLineBreak();
				result.Indent();
				result.Append("</");
				result.Append(name);
				result.Append('>');
			}
		}

		static bool ByPassComment(const std::string &content, size_t &pos)
		{
			if (StartWith(content, "<!--", pos))
			{
				pos += 4;
				while (!StartWith(content, "-->", pos))
					++pos;
				pos += 3;
				return true;
			}
			return false;
		}

		static void ByPass(const std::string &content, size_t &pos)
		{
			ByPassTrailing(content, pos);
			if (ByPassComment(content, pos))
				ByPass(content, pos);
		}

		static std::string LoadValue(const std::string &content, size_t &pos)
		{
			bool escaping = false;
			char current = content[pos];
			if (current != '<')
			{
				std::string value, escape;
				value += current;
				++pos;
				current = content[pos];
				while (current != '<' && pos != content.length())
				{
					if (escaping)
					{
						if (current != ';')
							escape += current;
						else
						{
							if (escape == "lt")
								value += '<';
							else if (escape == "gt")
								value += '>';
							else if (escape == "amp")
								value += '&';
							else if (escape == "apos")
								value += '\'';
							else if (escape == "quot")
								value += '"';
							escaping = false;
						}
						++pos;
					}
					else if (current == '\n')
					{
						value += current;
						ByPass(content, pos);
					}
					else if (current == '&')
					{
						escaping = true;
						++pos;
					}
					else
					{
						value += current;
						++pos;
					}
					current = content[pos];
				}
				if (pos == content.length())
					return "";
				value = value.substr(0, value.find_last_not_of(" \t\n") + 1);
				ByPass(content, pos);
				std::string add = LoadValue(content, pos);
				if (!add.empty())
					value += add;
				return value;
			}
			return "";
		}

		static Node LoadAttribute(const std::string &content, size_t &pos)
		{
			bool escaping = false;
			std::string name, value, escape;
			while (content[pos] != '=' && pos != content.length())
			{
				name += content[pos];
				++pos;
			}
			if (pos == content.length() ||
				(pos + 1) == content.length() ||
				content[pos + 1] != '"' ||
				(pos + 2) == content.length())
				return NULL_NODE;
			pos += 2;
			char current = content[pos];
			while (content[pos] != '"' && pos != content.length())
			{
				if (escaping)
				{
					if (current != ';')
						escape += current;
					else
					{
						if (escape == "lt")
							value += '<';
						else if (escape == "gt")
							value += '>';
						else if (escape == "amp")
							value += '&';
						else if (escape == "apos")
							value += '\'';
						else if (escape == "quot")
							value += '"';
						escaping = false;
					}
					++pos;
				}
				else if (current == '\n')
				{
					value += current;
					ByPass(content, pos);
				}
				else if (current == '&')
				{
					escaping = true;
					escape = "";
					++pos;
				}
				else
				{
					value += current;
					++pos;
				}
				current = content[pos];
			}
			if (pos == content.length())
				return NULL_NODE;
			value = value.substr(0, value.find_last_not_of(" \t\n") + 1);
			++pos;
			return Node(name, value);
		}

		static Node CreateNodeFromTag(const std::string &content, bool declarations)
		{
			size_t pos = 0;
			char current = content[pos];
			std::string name;
			name += current;
			++pos;
			current = content[pos];
			while (current != ' ' && current != '\t' && pos != content.length())
			{
				if (current == '>')
					return Node(name);
				name += current;
				++pos;
				current = content[pos];
			}
			Node node(name);
			if (pos == content.length())
				return node;
			ByPassTrailing(content, pos);
			current = content[pos];
			if (current == '>')
				return node;
			Node attributes("__xmlattributes__");
			while (current != '/' && current != '?' && current != '>' && current != '\0')
			{
				if (declarations)
					node.Add(LoadAttribute(content, pos));
				else
					attributes.Add(LoadAttribute(content, pos));
				ByPassTrailing(content, pos);
				current = content[pos];
			}
			if (!attributes.IsEmpty())
				node.Add(attributes);
			return node;
		}

		static Node LoadNode(const std::string &content, size_t &pos)
		{
			++pos;
			char current = content[pos];
			if (current == '/')
				return NULL_NODE;
			std::string nodeOpenTag;
			nodeOpenTag += current;
			++pos;
			current = content[pos];
			while (current != '>' && pos != content.length())
			{
				nodeOpenTag += current;
				++pos;
				current = content[pos];
			}
			if (pos == content.length())
				return NULL_NODE;
			Node node = CreateNodeFromTag(nodeOpenTag, false);
			++pos;
			ByPass(content, pos);
			if (nodeOpenTag[nodeOpenTag.length() - 1] == '/')
			{
				node.SetNull(true);
				return node;
			}
			std::string nodeValue, valueToAdd;
			while (!StartWith(content, "</", pos))
			{
				valueToAdd = LoadValue(content, pos);
				if (!valueToAdd.empty())
				{
					if (!nodeValue.empty())
						nodeValue += '\n';
					nodeValue += valueToAdd;
				}
				ByPass(content, pos);
				if (!StartWith(content, "</", pos))
				{
					Node child = LoadNode(content, pos);
					if (child.IsNullNode())
						return NULL_NODE;
					node.Add(child);
				}
				if (pos == content.length())
					return NULL_NODE;
			}
			node.SetValue(nodeValue);
			pos += 2;
			if (!StartWith(content, node.GetName(), pos))
				return NULL_NODE;
			pos += node.GetName().length() + 1;
			ByPass(content, pos);
			return node;
		}

		static Node LoadDeclaration(const std::string &content, size_t &pos)
		{
			++pos;
			char current = content[pos];
			if (current == '/')
				return NULL_NODE;
			std::string nodeOpenTag;
			nodeOpenTag += current;
			++pos;
			current = content[pos];
			while (current != '?' && pos != content.length())
			{
				nodeOpenTag += current;
				++pos;
				current = content[pos];
			}
			++pos;
			if (pos == content.length())
				return NULL_NODE;
			++pos;
			Node declaration = CreateNodeFromTag(nodeOpenTag, true);
			if (declaration.GetName() == "xml")
			{
				for (auto &child : declaration)
					child.SetIsString(true);
			}
			return declaration;
		}

		static Node LoadDeclarations(const std::string &content, size_t &pos)
		{
			Node declarations("__xmldeclarations__");
			while (StartWith(content, "<?", pos))
			{
				++pos;
				Node declaration = LoadDeclaration(content, pos);
				if (declaration.IsNullNode())
					return NULL_NODE;
				declarations.Add(declaration);
				ByPass(content, pos);
			}
			return declarations;
		}

		void Xml::Write(const Node &node, std::ostream &os, unsigned int indentFactor)
		{
			os << Str(node, indentFactor);
		}

		void Xml::Write(const Node &node, const std::string &filename, unsigned int indentFactor)
		{
			std::ofstream file(filename);
			file << Str(node, indentFactor);
		}

		std::string Xml::Str(const Node &node, unsigned int indentFactor, unsigned int depth, bool breakLine)
		{
			if (node.IsNullNode())
				return "(null)";
			Result result(Format{ indentFactor, depth, breakLine });
			Dp::Str(node, result);
			return result.Str();
		}

		std::string Xml::Str(const Node &node, const Format &format)
		{
			if (node.IsNullNode())
				return "(null)";
			Result result(format);
			Dp::Str(node, result);
			return result.Str();
		}

		Node Xml::LoadFromFile(const std::string &path)
		{
			struct stat buf;
			if (stat(path.c_str(), &buf) != 0)
				return NULL_NODE;
			std::ifstream fileStream(path.c_str());
			size_t nbChar = static_cast<size_t>(buf.st_size);
			auto buffer = new char[nbChar + 1];
			fileStream.read(buffer, nbChar);
			buffer[nbChar] = '\0';
			std::string content(buffer);
			Node node = LoadFromContent(content);
			delete[](buffer);
			if (node.IsNotNullNode() && node.GetNode("__xmldeclarations__.xml").IsNullNode())
				return NULL_NODE;
			return node;
		}

		Node Xml::LoadFromStream(std::istream &stream)
		{
			if (!stream.good())
				return NULL_NODE;
			std::string content, line;
			while (std::getline(stream, line))
			{
				if (!line.empty())
				{
					content += line;
					content += '\n';
				}
			}
			return LoadFromContent(content);
		}

		Node Xml::LoadFromContent(const std::string &content)
		{
			size_t pos = 0;
			ByPass(content, pos);
			Node declarations = LoadDeclarations(content, pos);
			if (declarations.IsNullNode())
				return NULL_NODE;
			ByPass(content, pos);
			if (content[pos] != '<')
				return NULL_NODE;
			Node node = LoadNode(content, pos);
			if (node.IsNullNode())
				return NULL_NODE;
			if (!declarations.IsEmpty())
				node.Add(declarations);
			return node;
		}
	}
}