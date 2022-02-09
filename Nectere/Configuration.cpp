#include "Configuration.hpp"

#include <filesystem>
#include <iostream>
#include "Dp/Json.hpp"
#include "Logger.hpp"
#include "Parameters/AuthParameter.hpp"
#include "Parameters/LogPathParameter.hpp"
#include "Parameters/VerboseParameter.hpp"
#include "parg/parg.h"
#include "Script/CodingStyle.hpp"

namespace Nectere
{
	Configuration::Configuration(): m_ShouldStartServer(true), m_ConfigurationFilePath("D:\\Programmation\\Nectere\\NectereConfig.json")
	{
		//Auth
		Add<Parameters::AuthParameter>();

		//Log
		Add<Parameters::VerboseParameter>();
		Add<Parameters::LogInFileParameter>();
		Add<Parameters::LogPathParameter>(dynamic_cast<Parameters::LogInFileParameter *>(Fetch("LogInFile")));

		//Script Engine
		Add<BoolParameter>("ScriptEngine.DoSmartBuild", true);
		Add<BoolParameter>("ScriptEngine.DoCodingStyle", false);
		Add<BoolParameter>("ScriptEngine.DisplayName", false);
		Add<BoolParameter>("ScriptEngine.DisplayTag", false);
	}

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

	static void callback_Callback(void *data, const char *name, const char *value, int)
	{
		((Configuration *)data)->Callback(name, value);
	}

	void Configuration::AParameter::AddToParg(parg *root, Configuration *configuration) const
	{
		for (const auto &pair : m_Restrictions)
		{
			configuration->m_ArgumentToParameter[pair.first] = GetName();
			parg_add_callback(root, pair.first.c_str(), &callback_Callback, configuration, pair.second.first, pair.second.second);
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
		return m_Parameters[parameter];
	}

	bool Configuration::Have(const std::string &parameter)
	{
		return (m_Parameters.find(parameter) != m_Parameters.end());
	}

	void Configuration::AddParameter(AParameter *parameter)
	{
		if (parameter != nullptr)
		{
			parameter->Init();
			m_Parameters[parameter->GetName()] = parameter;
		}
	}

	static void callback_ConfigFile(void *data, const char *, const char *value, int)
	{
		((Configuration *)data)->ConfigFile(value);
	}

	static void callback_Help(void *data, const char *, const char *, int)
	{
		((Configuration *)data)->Help();
	}

	bool Configuration::LoadConfiguration(int argc, char **argv)
	{
		parg *root = parg_alloc();
		for (const auto &pair : m_Parameters)
			pair.second->AddToParg(root, this);
		parg_add_callback(root, "config-file", &callback_ConfigFile, this, PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_add_callback(root, "help", &callback_Help, this, PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_add_callback(root, "h", &callback_Help, this, PARG_SIMPLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		parg_parse(root, argc, argv);
		parg_free(root);
		std::string configPath = std::filesystem::absolute(m_ConfigurationFilePath).string();
		m_LoadedConfig = Dp::Json::LoadFromFile(configPath);
		if (m_LoadedConfig.IsNotNullNode())
		{
			for (const auto &pair : m_Parameters)
			{
				auto parameter = pair.second;
				std::string nodeName = parameter->GetJsonName();
				if (!nodeName.empty())
				{
					if (m_LoadedConfig.Find(nodeName))
						parameter->LoadNode(m_LoadedConfig.GetNode(nodeName));
				}
			}
		}
//		if (ms_ShouldStartServer)
//			Script::CodingStyle::Init();
		return m_ShouldStartServer;
	}

	void Configuration::Clear()
	{
		for (const auto &pair : m_Parameters)
			delete(pair.second);
	}

	void Configuration::Callback(const char *name, const char *value)
	{
		if (name != nullptr)
		{
			auto it = m_ArgumentToParameter.find(std::string(name));
			if (it != m_ArgumentToParameter.end())
			{
				auto parameter = m_Parameters.find((*it).second);
				if (parameter != m_Parameters.end())
					(*parameter).second->ParseArgument(value);
			}
		}
	}

	void Configuration::Help()
	{
		m_ShouldStartServer = false;
		std::cout << "-h --help\t\t\tShow help on the command lines parameters allowed." << std::endl;
		for (const auto &pair : m_Parameters)
			pair.second->DumpHelp();
	}

	void Configuration::ConfigFile(const char *value)
	{
		m_ConfigurationFilePath = value;
		if (m_Logger)
			m_Logger->Log(LogType::Verbose, "Changed configuration file path to \"", m_ConfigurationFilePath, '\"');
	}
}