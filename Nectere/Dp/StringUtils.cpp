#include "Dp/StringUtils.hpp"

namespace Nectere
{
	namespace Dp
	{
		namespace StringUtils
		{
			bool StringUtils::Find(const std::string &str, char search, unsigned long &pos)
			{
				pos = 0;
				while (str[pos] != '\0')
				{
					if (str[pos] == search)
						return true;
					++pos;
				}
				return false;
			}

			bool StringUtils::Find(const std::string &str, char search)
			{
				unsigned long n;
				return Find(str, search, n);
			}
		}
	}
}