#pragma once

#include <string>
#include <vector>
#include "nectere_export.h"

namespace Nectere
{
	namespace StringUtils
	{
		NECTERE_EXPORT bool Find(const std::string &, char, unsigned long &);
		NECTERE_EXPORT bool Find(const std::string &, char);
		NECTERE_EXPORT bool StartWith(const std::string &, const std::string &);
		NECTERE_EXPORT bool EndWith(const std::string &, const std::string &);
		NECTERE_EXPORT void ReplaceAll(std::string &, const std::string &, const std::string &);
		NECTERE_EXPORT void Trim(std::string &);
		NECTERE_EXPORT size_t Count(const std::string &, const std::string &);
		NECTERE_EXPORT std::vector<std::string> Split(const std::string &, const std::string &, bool, bool);
		NECTERE_EXPORT inline std::vector<std::string> Split(const std::string &, const std::string &);
		NECTERE_EXPORT inline std::vector<std::string> Split(const std::string &, const std::string &, bool);
		NECTERE_EXPORT int SmartSplit(std::vector<std::string> &, const std::string &, const std::string &, bool);
		NECTERE_EXPORT int SmartSplit(std::vector<std::string> &, const std::string &, const std::string &);
	};
}