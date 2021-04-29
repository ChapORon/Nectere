#include "Dp/Json.hpp"

#include <fstream>
#include <regex>

namespace Nectere
{
	namespace Dp
	{
		Json::EJsonElementType Json::GetType(const Node &node)
		{
			if (!node.HaveChild())
				return EJsonElementType::Value;
			else if (node.HaveNamedChild())
				return EJsonElementType::Object;
			return EJsonElementType::Array;
		}

		void Json::StrString(Result &result, const std::string &value)
		{
			result.Append('"');
			for (char c : value)
			{
				switch (c)
				{
				case '\a': { result.Append("\\a"); break; }
				case '\b': { result.Append("\\b"); break; }
				case '\f': { result.Append("\\f"); break; }
				case '\n': { result.Append("\\n"); break; }
				case '\r': { result.Append("\\r"); break; }
				case '\t': { result.Append("\\t"); break; }
				case '\v': { result.Append("\\v"); break; }
				case '"': { result.Append("\\\""); break; }
				default: result.Append(c);
				}
			}
			result.Append('"');
		}

		void Json::StrObject(const Node &node, Result &result, char open, char close, bool name)
		{
			result.Append(open);
			result.LineBreak();
			result.IncreaseDepth();
			unsigned int n = 0;
			for (const auto &element : node)
			{
				if (n != 0)
				{
					result.Append(',');
					if (result.IsLineBreakAllowed())
						result.ForceLineBreak();
					else
						result.Append(' ');
				}
				Str(element, result, name);
				++n;
			}
			result.DecreaseDepth();
			result.LineBreak();
			result.Indent();
			result.Append(close);
		}

		void Json::StrValue(const Node &node, Result &result)
		{
			if (node.IsNull())
				result.Append("null");
			else
			{
				const std::string &value = node.GetValue();
				if (node.IsString() || (!node.IsNotAString() && !node.IsBoolean() && !node.IsNumerical()))
					StrString(result, value);
				else
					result.Append(value);
			}
		}

		void Json::Str(const Node &node, Result &result, bool addName)
		{
			result.Indent();
			if (addName)
			{
				const std::string &name = node.GetName();
				if (!name.empty())
				{
					result.Append('"');
					result.Append(name);
					result.Append("\": ");
				}
			}
			switch (GetType(node))
			{
			case EJsonElementType::Array: { StrObject(node, result, '[', ']', false); break; }
			case EJsonElementType::Object: { StrObject(node, result, '{', '}', true); break; }
			case EJsonElementType::Value: { StrValue(node, result); break; }
			}
		}

		void Json::Str(const Node &node, Result &result)
		{
			if (!node.GetName().empty() && result.GetDepth() == 0)
			{
				Node tmp;
				tmp.Emplace(node);
				Str(tmp, result, false);
			}
			else
				Str(node, result, true);
		}

		std::string Json::LoadString(const std::string &content, size_t &pos)
		{
			char current = content[pos];
			if (current != '"')
			{
				bool escaping = false;
				std::stringstream value;
				if (current == '\\')
					escaping = true;
				else
					value << current;
				++pos;
				current = content[pos];
				while ((current != '"' || escaping) && pos != content.length())
				{
					if (escaping)
					{
						switch (current)
						{
						case 'a': { value << '\a'; break; }
						case 'b': { value << '\b'; break; }
						case 'f': { value << '\f'; break; }
						case 'n': { value << '\n'; break; }
						case 'r': { value << '\r'; break; }
						case 't': { value << '\t'; break; }
						case 'v': { value << '\v'; break; }
						case '"': { value << '"'; break; }
						default: { value << '\\' << current; break; }
						}
						escaping = false;
					}
					else if (current == '\\')
						escaping = true;
					else
						value << current;
					++pos;
					current = content[pos];
				}
				if (pos == content.length())
					return "";
				++pos;
				return value.str();
			}
			++pos;
			return "";
		}

		Node Json::LoadValue(const std::string &content, size_t &pos, const std::string &name)
		{
			Node ret(name);
			char current = content[pos];
			if (current == '"')
			{
				++pos;
				std::string value = LoadString(content, pos);
				if (pos == content.length())
					return Node::null;
				ret.SetValue(value);
				ret.SetIsString(true);
				return ret;
			}
			else if (current == 'n')
			{
				if (pos + 3 < content.length() &&
					content[pos + 1] == 'u' &&
					content[pos + 2] == 'l' &&
					content[pos + 3] == 'l')
				{
					pos += 4;
					ret.SetNull(true);
					return ret;
				}
				return Node::null;
			}
			else
			{
				bool haveChar = false;
				std::stringstream value;
				while (!IsWhitespace(current) &&
					current != ',' &&
					current != ']' &&
					current != '}' &&
					pos != content.length())
				{
					value << current;
					++pos;
					current = content[pos];
					haveChar = true;
				}
				if (pos == content.length() || !haveChar)
					return Node::null;
				ret.SetValue(value.str());
				if (!ret.IsNumerical() && !ret.IsBoolean())
					return Node::null;
				ret.SetIsNotAString(true);
				return ret;
			}
		}

		Node Json::LoadArray(const std::string &content, size_t &pos, const std::string &name)
		{
			Node ret(name);
			bool needMore = true;
			char current = content[pos];
			while (current != ']' && pos != content.length())
			{
				if (needMore)
				{
					needMore = false;
					Node value = LoadNode(content, pos, "");
					if (value == Node::null)
						return value;
					ret.Emplace(value);
					current = content[pos];
					if (current == ',')
					{
						needMore = true;
						++pos;
						ByPassTrailing(content, pos);
						current = content[pos];
					}
					else
					{
						ByPassTrailing(content, pos);
						current = content[pos];
					}
				}
				else
					return Node::null;
			}
			if (pos == content.length() || needMore)
				return Node::null;
			++pos;
			ByPassTrailing(content, pos);
			return ret;
		}

		Node Json::LoadObject(const std::string &content, size_t &pos, const std::string &name)
		{
			Node ret(name);
			char current = content[pos];
			switch (current)
			{
			case '}':
			{
				++pos;
				return ret;
			}
			case '"':
			{
				bool needMore = true;
				while (current != '}' && pos != content.length())
				{
					if (needMore && current == '"')
					{
						++pos;
						needMore = false;
						std::string name = LoadString(content, pos);
						if (pos == content.length() || name.empty())
							return Node::null;
						ByPassTrailing(content, pos);
						Node value = LoadNode(content, pos, name);
						if (value == Node::null)
							return Node::null;
						ret.Add(value);
						current = content[pos];
						if (current == ',')
						{
							needMore = true;
							++pos;
							ByPassTrailing(content, pos);
							current = content[pos];
						}
						else
						{
							ByPassTrailing(content, pos);
							current = content[pos];
						}
					}
					else
						return Node::null;
				}
				if (pos == content.length() || needMore)
					return Node::null;
				++pos;
				ByPassTrailing(content, pos);
				return ret;
			}
			}
			return Node::null;
		}

		Node Json::LoadNode(const std::string &content, size_t &pos, const std::string &name)
		{
			char current = content[pos];
			++pos;
			ByPassTrailing(content, pos);
			switch (current)
			{
			case ':':
			{
				switch (content[pos])
				{
				case '{':
				case '[':
					return LoadNode(content, pos, name);
				default:
				{
					if (!name.empty())
						return LoadValue(content, pos, name);
					return Node::null;
				}
				}
			}
			case '{':
				return LoadObject(content, pos, name);
			case '[':
				return LoadArray(content, pos, name);
			}
			if (name.empty())
				return LoadValue(content, pos, name);
			return Node::null;
		}

		bool Json::StartWith(const std::string &str, const std::string &search, size_t pos)
		{
			for (unsigned long n = 0; n != search.length(); ++n)
			{
				if ((n + pos) > str.length() ||
					str[n + pos] != search[n])
					return false;
			}
			return true;
		}

		bool Json::IsWhitespace(char c)
		{
			return (c == ' ' || c == '\t' || c == '\n');
		}

		void Json::ByPassTrailing(const std::string &content, size_t &pos)
		{
			char current = content[pos];
			while (IsWhitespace(current))
			{
				++pos;
				current = content[pos];
			}
		}

		void Json::Write(const Node &node, std::ostream &os, unsigned int indentFactor)
		{
			os << Str(node, indentFactor);
		}

		void Json::Write(const Node &node, const std::string &filename, unsigned int indentFactor)
		{
			std::ofstream file(filename);
			file << Str(node, indentFactor);
		}

		std::string Json::Str(const Node &node, unsigned int indentFactor, unsigned int depth, bool breakLine)
		{
			if (node == Node::null)
				return "(null)";
			Result result(Format{ indentFactor, depth, breakLine });
			Str(node, result);
			return result.Str();
		}

		std::string Json::Str(const Node &node, const Format &format)
		{
			if (node == Node::null)
				return "(null)";
			Result result(format);
			Str(node, result);
			return result.Str();
		}

		Node Json::LoadFromFile(const std::string &path)
		{
			struct stat buf;
			if (stat(path.c_str(), &buf) != 0)
				return Node::null;
			std::ifstream fileStream(path.c_str());
			size_t nbChar = static_cast<size_t>(buf.st_size);
			auto buffer = new char[nbChar + 1];
			fileStream.read(buffer, nbChar);
			buffer[nbChar] = '\0';
			std::string content(buffer);
			return LoadFromContent(content);
		}

		Node Json::LoadFromStream(std::istream &stream)
		{
			if (!stream.good())
				return Node::null;
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

		Node Json::LoadFromContent(const std::string &jsonContent)
		{
			size_t pos = 0;
			if (jsonContent.empty())
				return Node::null;
			std::string content = jsonContent;
			ByPassTrailing(content, pos);
			if (content[pos] != '{')
				return Node::null;
			return LoadNode(jsonContent, pos, "");
		}
	}
}