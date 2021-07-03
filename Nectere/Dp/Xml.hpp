#pragma once

#include "Dp/Format.hpp"
#include "Dp/Node.hpp"
#include "Dp/Result.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
    namespace Dp
    {
        class Xml
        {
        private:
            static Xml ms_Parser;

        public:
            static Node Create(const std::string &, const std::string & = "1.0", const std::string & = "UTF-8");
            template <typename t_AttributeType>
            static void AddAttribute(Node &node, const std::string &key, const t_AttributeType &value)
            {
                if (StringUtils::Find(key, '.'))
                    return;
                std::string attributeKey = "__xmlattributes__.";
                attributeKey += key;
                node.Add(attributeKey, value);
            }
			static void AddDeclaration(Node &, const std::string &, const std::string &, const std::string &);
			static std::string Str(const Node &, const Format &);
            static std::string Str(const Node &, unsigned int = 2, unsigned int = 0, bool = true);
            static void Write(const Node &, std::ostream &, unsigned int = 2);
            static void Write(const Node &, const std::string &, unsigned int = 2);
            static Node LoadFromFile(const std::string &);
            static Node LoadFromStream(std::istream &);
            static Node LoadFromContent(const std::string &);

		private:
			static void Str(const Node &, Result &);
			static bool StartWith(const std::string &, const std::string &, size_t);
			static bool IsWhitespace(char);
			static void ByPassTrailing(const std::string &, size_t &);
            static void AddValue(Result &, const std::string &);
            static bool AddDeclarations(const Node &, Result &);
            static bool AddAttributes(const Node &, Result &);
            static void AddNodes(const Node &, Result &);
            static Node LoadAttribute(const std::string &, size_t &);
            static std::string LoadValue(const std::string &, size_t &);
            static Node LoadNode(const std::string &, size_t &);
            static Node LoadDeclaration(const std::string &, size_t &);
            static Node LoadDeclarations(const std::string &, size_t &);
            static Node CreateNodeFromTag(const std::string &, bool);
            static bool ByPassComment(const std::string &, size_t &);
            static void ByPass(const std::string &, size_t &);
        };
    }
}
