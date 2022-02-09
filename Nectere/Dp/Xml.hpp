#pragma once

#include "Dp/Format.hpp"
#include "Dp/Node.hpp"
#include "Dp/Result.hpp"
#include "Dp/StringUtils.hpp"

namespace Nectere
{
    namespace Dp
    {
        namespace Xml
        {
            NECTEREDP_EXPORT Node Create(const std::string &, const std::string & = "1.0", const std::string & = "UTF-8");
            template <typename t_AttributeType>
            NECTEREDP_EXPORT void AddAttribute(Node &, const std::string &, const t_AttributeType &);
            NECTEREDP_EXPORT void AddDeclaration(Node &, const std::string &, const std::string &, const std::string &);
            NECTEREDP_EXPORT std::string Str(const Node &, const Format &);
            NECTEREDP_EXPORT std::string Str(const Node &, unsigned int = 2, unsigned int = 0, bool = true);
            NECTEREDP_EXPORT void Write(const Node &, std::ostream &, unsigned int = 2);
            NECTEREDP_EXPORT void Write(const Node &, const std::string &, unsigned int = 2);
            NECTEREDP_EXPORT Node LoadFromFile(const std::string &);
            NECTEREDP_EXPORT Node LoadFromStream(std::istream &);
            NECTEREDP_EXPORT Node LoadFromContent(const std::string &);
        };
    }
}
