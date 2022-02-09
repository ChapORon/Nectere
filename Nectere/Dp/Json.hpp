#pragma once

#include "Dp/Format.hpp"
#include "Dp/Node.hpp"
#include "Dp/Result.hpp"

namespace Nectere
{
    namespace Dp
    {
        namespace Json
        {
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
