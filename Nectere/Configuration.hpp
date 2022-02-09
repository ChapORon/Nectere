#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "nectere_export.h"
#include "Dp/Node.hpp"
#include "parg/parg_def.h"

struct parg;
namespace Nectere
{
	class Logger;
	class Configuration final
	{
	private:
		class AParameter
		{
			friend class Configuration;
		private:
			std::string m_Name;
			std::string m_JsonNode;
			std::unordered_map<std::string, std::pair<int, int>> m_Restrictions;

		private:
			virtual void Init() = 0;
			virtual std::string GetDescription() const { return ""; };
			virtual void ParseArgument(const char *str) = 0;
			virtual void LoadNode(const Dp::Node &node) = 0;
			void AddToParg(parg *, Configuration *) const;
			void DumpHelp() const;

		public:
			AParameter(const std::string &);
			AParameter(const std::string &, const std::string &);
			void AddRestriction(const std::string &, int, int);
			const std::string &GetName() const { return m_Name; }
			const std::string &GetJsonName() const { return m_JsonNode; }
		};

	public:
		template <typename t_ParameterType>
		class ATypedParameter : public AParameter
		{
		private:
			t_ParameterType m_Value;

		private:
			virtual t_ParameterType DefaultValue() const = 0;
			virtual t_ParameterType Parse() const { return DefaultValue(); }
			virtual t_ParameterType Parse(const std::string &) const { return DefaultValue(); }
			virtual t_ParameterType Load(const Dp::Node &node) const { return static_cast<t_ParameterType>(node); }

			void Init() override
			{
				m_Value = DefaultValue();
			}

			void ParseArgument(const char *str) override
			{
				if (str != nullptr)
					m_Value = Parse(std::string(str));
				else
					m_Value = Parse();
			}

			void LoadNode(const Dp::Node &node) override
			{
				m_Value = Load(node);
			}

		public:
			ATypedParameter(const std::string &name) : AParameter(name) {}
			ATypedParameter(const std::string &name, const std::string &node) : AParameter(name, node) {}
			const t_ParameterType &GetValue() const { return m_Value; }
			void ForceSet(const t_ParameterType &newValue) { m_Value = newValue; }
		};

		class BoolParameter final : public ATypedParameter<bool>
		{
		private:
			bool m_DefaultValue;

		private:
			bool DefaultValue() const override;
			bool Parse() const override;
			bool Parse(const std::string &) const override;

		public:
			BoolParameter(const std::string &, bool);
			BoolParameter(const std::string &, const std::string &, bool);
		};

	private:
		bool m_ShouldStartServer;
		const Logger *m_Logger = nullptr;
		std::string m_ConfigurationFilePath;
		std::unordered_map<std::string, std::string> m_ArgumentToParameter;
		std::unordered_map<std::string, AParameter *> m_Parameters;
		Dp::Node m_LoadedConfig;

	private:
		void AddParameter(AParameter *);

	public:
		Configuration();
		const Dp::Node &GetLoadedConfig() const { return m_LoadedConfig; }
		void SetLogger(const Logger *logger) { m_Logger = logger; }
		void Callback(const char *, const char *);
		void Help();
		void ConfigFile(const char *);

		template <typename t_ParameterType>
		bool Is(const std::string &parameter, const t_ParameterType &value)
		{
			auto it = m_Parameters.find(parameter);
			if (it != m_Parameters.end())
				return dynamic_cast<ATypedParameter<t_ParameterType> *>((*it).second)->GetValue() == value;
			return false;
		}

		template <typename t_ParameterType>
		const t_ParameterType &Get(const std::string &parameter)
		{
			return dynamic_cast<ATypedParameter<t_ParameterType> *>(m_Parameters[parameter])->GetValue();
		}

		AParameter *Fetch(const std::string &);

		template <typename t_ParameterType, typename ...t_Arg>
		void Add(t_Arg&&... args) { AddParameter(new t_ParameterType(std::forward<t_Arg>(args)...)); }

		bool Have(const std::string &parameter);
		bool LoadConfiguration(int argc, char **arg);
		void Clear();
	};
}