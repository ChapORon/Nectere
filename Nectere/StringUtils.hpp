#pragma once

#include <string>
#include <vector>

namespace Nectere
{
	class StringUtils final
	{
	private:
		static int SmartSplitCheckSubStr(const std::string &);

	public:
		static bool Find(const std::string &, char, unsigned long &);
		static bool Find(const std::string &, char);
		static bool StartWith(const std::string &, const std::string &);
		static bool EndWith(const std::string &, const std::string &);
		static void ReplaceAll(std::string &, const std::string &, const std::string &);
		static void Trim(std::string &);
		static size_t Count(const std::string &, const std::string &);
		static std::vector<std::string> Split(const std::string &, const std::string &, bool, bool);
		static inline std::vector<std::string> Split(const std::string &str, const std::string &regex) { return Split(str, regex, false, true); }
		static inline std::vector<std::string> Split(const std::string &str, const std::string &regex, bool trim) { return Split(str, regex, trim, true); }
		static int SmartSplit(std::vector<std::string> &, const std::string &, const std::string &, bool);
		static int SmartSplit(std::vector<std::string> &vec, const std::string &str, const std::string &search) { return SmartSplit(vec, str, search, false); }
	};
}