#pragma once

#include "Dp/Format.hpp"
#include "Dp/Node.hpp"
#include "Dp/Result.hpp"

namespace Nectere
{
    namespace Dp
    {
        class Json
        {
        private:
            enum class EJsonElementType : int
            {
                Object,
                Value,
                Array,
                Count,
                Invalid
            };

        private:
            static Json ms_Parser;

        public:
			static std::string Str(const Node &, const Format &);
            static std::string Str(const Node &, unsigned int = 2, unsigned int = 0, bool = true);
            static void Write(const Node &, std::ostream &, unsigned int = 2);
            static void Write(const Node &, const std::string &, unsigned int = 2);
            static Node LoadFromFile(const std::string &);
            static Node LoadFromStream(std::istream &);
            static Node LoadFromContent(const std::string &);

		private:
			static void Str(const Node &, Result &, bool);
			static void Str(const Node &, Result &);
			static bool StartWith(const std::string &, const std::string &, size_t);
			static bool IsWhitespace(char);
			static void ByPassTrailing(const std::string &, size_t &);
            static EJsonElementType GetType(const Node &);
            static void StrString(Result &, const std::string &);
            static void StrObject(const Node &, Result &, char, char, bool);
            static void StrValue(const Node &, Result &);
            static std::string LoadString(const std::string &, size_t &);
            static Node LoadValue(const std::string &, size_t &, const std::string &);
            static Node LoadArray(const std::string &, size_t &, const std::string &);
            static Node LoadObject(const std::string &, size_t &, const std::string &);
            static Node LoadNode(const std::string &, size_t &, const std::string &);
        };
    }
}
