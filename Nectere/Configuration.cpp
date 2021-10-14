#include "Configuration.hpp"

#include <filesystem>
#include <iostream>
#include "Dp/Json.hpp"
#include "Logger.hpp"
#include "parg/parg.h"
#include "Script/CodingStyle.hpp"

namespace Nectere
{
	bool Configuration::ms_ShouldStartServer = true;
	std::string Configuration::ms_ConfigurationFilePath = "conf.json";
	std::unordered_map<std::string, std::string> Configuration::ms_ArgumentToParameter;
	std::unordered_map<std::string, Configuration::AParameter *> Configuration::ms_Parameters;

	bool Configuration::BoolParameter::DefaultValue() const { return m_DefaultValue; }
	bool Configuration::BoolParameter::Parse() const { return true; }
	bool Configuration::BoolParameter::Parse(const std::string &value) const { return std::stoi(value); }
	Configuration::BoolParameter::BoolParameter(const std::string &name, bool defaultValue) : ATypedParameter<bool>(name), m_DefaultValue(defaultValue) {}
	Configuration::BoolParameter::BoolParameter(const std::string &name, const std::string &node, bool defaultValue) : ATypedParameter<bool>(name, node), m_DefaultValue(defaultValue) {}

	Configuration::AParameter::AParameter(const std::string &name) : m_Name(name) {}
	Configuration::AParameter::AParameter(const std::string &name, const std::string &node) : m_Name(name), m_JsonNode(node) {}

	void Configuration::AParameter::AddRestriction(const std::string &argument, int restriction, int valueRestriction)
	{
		m_Restrictions[argument] = std::pair(restriction, valueRestriction);
	}

	void Configuration::AParameter::AddToParg(parg *root) const
	{
		for (const auto &pair : m_Restrictions)
		{
			ms_ArgumentToParameter[pair.first] = GetName();
			parg_add_callback(root, pair.first.c_str(), &Configuration::Callback, pair.second.first, pair.second.second);
		}
	}

	void Configuration::AParameter::DumpHelp() const
	{
		if (!m_Restrictions.empty())
		{
			std::string description = GetDescription();
			if (!description.empty())
				std::cout << GetDescription() << std::endl;
			else
			{
				unsigned int n = 0;
				for (const auto &pair : m_Restrictions)
				{
					std::string arg = pair.first;
					int restriction = pair.second.first;
					if (n != 0)
						std::cout << " ";
					if (restriction == PARG_DOUBLE_DASH_RESTRICTION)
						std::cout << "--" << arg << " ";
					else if (restriction == PARG_DOUBLE_DASH_RESTRICTION)
						std::cout << '-' << arg << " ";
					else
						std::cout << '-' << arg << " --" << arg << " ";
					++n;
				}
				std::cout << std::endl;
			}
		}
	}

	Configuration::AParameter *Configuration::Fetch(const std::string &parameter)
	{
		return ms_Parameters[parameter];
	}

	bool Configuration::Have(const std::string &parameter)
	{
		return (ms_Parameters.find(parameter) != ms_Parameters.end());
	}

	void Configuration::AddParameter(AParameter *parameter)
	{
		if (parameter != nullptr)
		{
			parameter->Init();
			ms_Parameters[parameter->GetName()] = parameter;
		}
	}

	bool Configuration::LoadConfiguration(int argc, char **argv)
	{
		parg *root = parg_alloc();
		for (const auto &pair : ms_Parameters)
			pair.second->AddToParg(root);
		parg_add_callback(root, "config-file", &Configuration::ConfigFile, PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_add_callback(root, "help", &Configuration::Help, PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_add_callback(root, "h", &Configuration::Help, PARG_SIMPLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_parse(root, argc, argv);
		parg_free(root);
		std::string configPath = std::filesystem::absolute(ms_ConfigurationFilePath).string();
		Dp::Node node = Dp::Json::LoadFromFile(configPath);
		if (!node.IsNull())
		{
			for (const auto &pair : ms_Parameters)
			{
				auto parameter = pair.second;
				std::string nodeName = parameter->GetJsonName();
				if (!nodeName.empty())
				{
					if (node.Find(nodeName))
						parameter->LoadNode(node.GetNode(nodeName));
				}
			}
		}
//		if (ms_ShouldStartServer)
//			Script::CodingStyle::Init();
		return ms_ShouldStartServer;
	}

	void Configuration::Clear()
	{
		for (const auto &pair : ms_Parameters)
			delete(pair.second);
	}

	void Configuration::Callback(const char *name, const char *value, int)
	{
		if (name != nullptr)
		{
			auto it = ms_ArgumentToParameter.find(std::string(name));
			if (it != ms_ArgumentToParameter.end())
			{
				auto parameter = ms_Parameters.find((*it).second);
				if (parameter != ms_Parameters.end())
					(*parameter).second->ParseArgument(value);
			}
		}
	}

	void Configuration::Help(const char *, const char *, int)
	{
		ms_ShouldStartServer = false;
		std::cout << "-h --help\t\t\tShow help on the command lines parameters allowed." << std::endl;
		for (const auto &pair : ms_Parameters)
			pair.second->DumpHelp();
	}

	void Configuration::ConfigFile(const char *, const char *value, int)
	{
		ms_ConfigurationFilePath = value;
		LOG(LogType::Verbose, "Changed configuration file path to \"", ms_ConfigurationFilePath, '\"');
	}
}