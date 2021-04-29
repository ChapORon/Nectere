#pragma once

#include <string>
#include <unordered_map>

namespace Nectere
{
	class Configuration final
	{
	private:
		static std::unordered_map<std::string, bool> ms_BoolParameters;
		static std::unordered_map<std::string, int> ms_IntParameters;

	public:
		static bool Is(const std::string &, bool);
		static bool GetBool(const std::string &);
		static void Set(const std::string &, bool);
		static bool Is(const std::string &, int);
		static int GetInt(const std::string &);
		static void Set(const std::string &, int);
	};
}