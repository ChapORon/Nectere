#include <regex>
#include <sstream>
#include "StringUtils.hpp"

namespace Nectere
{
	bool StringUtils::StartWith(const std::string &str, const std::string &search)
	{
		if (str == search)
			return true;
		else if (str.size() == 0)
			return false;
		for (unsigned long n = 0; n != search.length(); ++n)
		{
			if (n >= str.length() || str[n] != search[n])
				return false;
		}
		return true;
	}

	bool StringUtils::EndWith(const std::string &str, const std::string &search)
	{
		if (str == search)
			return true;
		else if (str.size() == 0)
			return false;
		size_t strLen = str.length();
		size_t searchLen = search.length();
		for (size_t n = searchLen; n != 0; --n)
		{
			size_t strPos = strLen - n;
			size_t searchPos = searchLen - n;
			if ((strLen - n) < 0 || str[strPos] != search[searchPos])
				return false;
		}
		return (str[strLen] == search[searchLen]);
	}

	void StringUtils::ReplaceAll(std::string &str, const std::string &search, const std::string &replace)
	{
		if (str.size() == 0)
			return;
		size_t pos;
		while ((pos = str.find(search, 0)) != std::string::npos)
			str.replace(pos, search.length(), replace);
	}

	void StringUtils::Trim(std::string &str)
	{
		if (str.size() == 0)
			return;
		ReplaceAll(str, "\t", " ");
		ReplaceAll(str, "  ", " ");
		if (str[0] == ' ')
			str = str.substr(1);
		if (str[str.length() - 1] == ' ')
			str = str.substr(0, str.length() - 1);
	}

	size_t StringUtils::Count(const std::string &str, const std::string &regex)
	{
		if (str == regex)
			return 1;
		else if (str.size() == 0)
			return 0;
		std::string subject = str;
		std::smatch match;
		std::regex re(regex);
		int i = 0;
		while (std::regex_search(subject, match, re))
		{
			++i;
			subject = match.suffix().str();
		}
		return i;
	}

	std::vector<std::string> StringUtils::Split(const std::string &str, const std::string &regex, bool trim, bool addEmpty)
	{
		std::vector<std::string> vec;
		std::regex re(regex);
		std::sregex_token_iterator it(str.begin(), str.end(), re, -1);
		std::sregex_token_iterator end;
		while (it != end)
		{
			std::string substr(*it++);
			if (trim)
				Trim(substr);
			if (addEmpty || substr != "")
				vec.emplace_back(substr);
		}
		return vec;
	}

	static int SmartSplitCheckSubStr(const std::string &str)
	{
		size_t nbClosingBracketInStr = 0;
		size_t nbOpeningBracketInStr = 0;
		size_t nbQuoteMarkInStr = 0;
		for (char c : str)
		{
			if (c == '"')
				++nbQuoteMarkInStr;
			else if (c == '}' && (nbQuoteMarkInStr % 2) == 0)
			{
				if (nbOpeningBracketInStr == nbClosingBracketInStr)
					return 1; //Close bracket not opened
				++nbClosingBracketInStr;
			}
			else if (c == '{' && (nbQuoteMarkInStr % 2) == 0)
				++nbOpeningBracketInStr;
		}
		if (nbClosingBracketInStr < nbOpeningBracketInStr)
			return 2; //Open bracket not closed
		else if ((nbQuoteMarkInStr % 2) != 0)
			return 3; //Quotes not closed
		return 0;
	}

	int StringUtils::SmartSplit(std::vector<std::string> &vec, const std::string &str, const std::string &search, bool appendSearch)
	{
		if (str.size() == 0)
			return 0;
		else if (str == search)
		{
			int check = SmartSplitCheckSubStr(str);
			if (check == 0)
				vec.emplace_back(str);
			return check;
		}
		size_t searchSize = search.size();
		size_t prev_pos = 0, pos = 0;
		while ((pos = str.find(search, pos)) != std::string::npos)
		{
			size_t subStrSize = pos - prev_pos;
			if (appendSearch)
				subStrSize += searchSize;
			std::string substr(str.substr(prev_pos, subStrSize));
			Trim(substr);
			int check = SmartSplitCheckSubStr(substr);
			if (check == 1)
				return 1;
			if (check == 0)
			{
				if (substr.size() != 0)
					vec.emplace_back(substr);
				prev_pos = pos + searchSize;
			}
			pos += searchSize;
		}
		size_t subStrSize = pos - prev_pos;
		if (appendSearch)
			subStrSize += searchSize;
		std::string substr(str.substr(prev_pos, subStrSize));
		Trim(substr);
		int check = SmartSplitCheckSubStr(substr);
		if (check == 0 && substr.size() != 0)
			vec.emplace_back(substr);
		return check;
	}

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

	std::vector<std::string> StringUtils::Split(const std::string &str, const std::string &regex) { return Split(str, regex, false, true); }
	std::vector<std::string> StringUtils::Split(const std::string &str, const std::string &regex, bool trim) { return Split(str, regex, trim, true); }
	int StringUtils::SmartSplit(std::vector<std::string> &vec, const std::string &str, const std::string &search) { return SmartSplit(vec, str, search, false); }
}