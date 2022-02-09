#include "Dp/Json.hpp"

#include <fstream>
#include <regex>

namespace Nectere
{
	namespace Dp
	{
		const Node NULL_NODE = Node(true);

		void Str(const Node &, Result &);
		void Str(const Node &, Result &, bool);
		Node LoadNode(const std::string &, size_t &, const std::string &);

		enum class EJsonElementType : int
		{
			Object,
			Value,
			Array,
			Count,
			Invalid
		};

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

		static EJsonElementType GetType(const Node &node)
		{
			if (node.IsArray())
				return EJsonElementType::Array;
			else if (!node.HaveChild())
				return EJsonElementType::Value;
			return EJsonElementType::Object;
		}

		static void StrString(Result &result, const std::string &value)
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

		static void StrObject(const Node &node, Result &result, char open, char close, bool name)
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

		static void StrValue(const Node &node, Result &result)
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

		static void Str(const Node &node, Result &result, bool addName)
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

		static void Str(const Node &node, Result &result)
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

		static std::string LoadString(const std::string &content, size_t &pos)
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

		static Node LoadValue(const std::string &content, size_t &pos, const std::string &name)
		{
			Node ret(name);
			char current = content[pos];
			if (current == '"')
			{
				++pos;
				std::string value = LoadString(content, pos);
				if (pos == content.length())
					return NULL_NODE;
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
				return NULL_NODE;
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
					return NULL_NODE;
				ret.SetValue(value.str());
				if (!ret.IsNumerical() && !ret.IsBoolean())
					return NULL_NODE;
				ret.SetIsNotAString(true);
				return ret;
			}
		}

		static Node LoadArray(const std::string &content, size_t &pos, const std::string &name)
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
					if (value.IsNullNode())
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
					return NULL_NODE;
			}
			if (pos == content.length() || needMore)
				return NULL_NODE;
			++pos;
			ByPassTrailing(content, pos);
			return ret;
		}

		static Node LoadObject(const std::string &content, size_t &pos, const std::string &name)
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
							return NULL_NODE;
						ByPassTrailing(content, pos);
						Node value = LoadNode(content, pos, name);
						if (value.IsNullNode())
							return NULL_NODE;
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
						return NULL_NODE;
				}
				if (pos == content.length() || needMore)
					return NULL_NODE;
				++pos;
				ByPassTrailing(content, pos);
				return ret;
			}
			}
			return NULL_NODE;
		}

		static Node LoadNode(const std::string &content, size_t &pos, const std::string &name)
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
					return NULL_NODE;
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
			return NULL_NODE;
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
			if (node.IsNullNode())
				return "(null)";
			Result result(Format{ indentFactor, depth, breakLine });
			Dp::Str(node, result);
			return result.Str();
		}

		std::string Json::Str(const Node &node, const Format &format)
		{
			if (node.IsNullNode())
				return "(null)";
			Result result(format);
			Dp::Str(node, result);
			return result.Str();
		}

		Node Json::LoadFromFile(const std::string &path)
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
			Node ret = LoadFromContent(content);
			delete[](buffer);
			return ret;
		}

		Node Json::LoadFromStream(std::istream &stream)
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

		Node Json::LoadFromContent(const std::string &jsonContent)
		{
			size_t pos = 0;
			if (jsonContent.empty())
				return NULL_NODE;
			std::string content = jsonContent;
			ByPassTrailing(content, pos);
			if (content[pos] != '{')
				return NULL_NODE;
			return LoadNode(jsonContent, pos, "");
		}
	}
}