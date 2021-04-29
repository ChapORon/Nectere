#include "Configuration.hpp"

namespace Nectere
{
	std::unordered_map<std::string, bool> Configuration::ms_BoolParameters;

	bool Configuration::Is(const std::string &name, bool value)
	{
		auto it = ms_BoolParameters.find(name);
		if (it != ms_BoolParameters.end())
			return (*it).second == value;
		return false;
	}

	bool Configuration::GetBool(const std::string &name)
	{
		return ms_BoolParameters[name];
	}

	void Configuration::Set(const std::string &name, bool value)
	{
		ms_BoolParameters[name] = value;
	}

	std::unordered_map<std::string, int> Configuration::ms_IntParameters;

	bool Configuration::Is(const std::string &name, int value)
	{
		auto it = ms_IntParameters.find(name);
		if (it != ms_IntParameters.end())
			return (*it).second == value;
		return false;
	}

	int Configuration::GetInt(const std::string &name)
	{
		return ms_IntParameters[name];
	}

	void Configuration::Set(const std::string &name, int value)
	{
		ms_IntParameters[name] = value;
	}
}