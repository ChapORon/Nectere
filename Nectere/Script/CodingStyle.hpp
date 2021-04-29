#pragma once

#include <string>

namespace Nectere
{
	namespace Script
	{
		class CodingStyle
		{
			friend class Configuration;
		public:
			enum class NameType : char
			{
				Variable,
				Global,
				DeclaredObject,
				Function,
				Count,
				Invalid
			};

			enum class CasePreset : char
			{
				CamelCase, //camelCase
				PascalCase, //PascalCase
				SnakeCase, //snake_case
				ScreamingSnakeCase, //SCREAMING_SNAKE_CASE
				KebabCase, //kebab-case
				Count,
				Invalid
			};
		private:
			static CodingStyle ms_CodingStyle;
			static bool ms_DoCodingStyle;
			bool (*m_VariableChecker)(const std::string &) { nullptr };
			bool (*m_GlobalChecker)(const std::string &) { nullptr };
			bool (*m_FunctionChecker)(const std::string &) { nullptr };
			bool (*m_DeclaredObjectChecker)(const std::string &) { nullptr };

		private:
			bool IsNameCompliant(const NameType &, const std::string &);
			bool SetChecker(const NameType &, bool (*)(const std::string &));
			static void Init();
			static void SetDefaultCodingStyle();
			static const char *ErrorNameOf(const NameType &);

		public:
			static bool SetCheckerFor(const NameType &, const CasePreset &);
			static bool SetCheckerFor(const NameType &, bool (*)(const std::string &));
			static bool CheckName(const NameType &, const std::string &);
		};
	}
}