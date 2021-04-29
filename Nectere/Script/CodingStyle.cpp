#include "Script/CodingStyle.hpp"

#include <regex>
#include "Configuration.hpp"
#include "Logger.hpp"

constexpr char VARIABLE_ERROR_NAME[] = "Variable";
constexpr char GLOBAL_ERROR_NAME[] = "Global";
constexpr char FUNCTION_ERROR_NAME[] = "Function";
constexpr char DECLARED_OBJECT_ERROR_NAME[] = "Declared object";

static std::regex SNAKE_CASE_REGEX("[a-z]+(_([a-z]*))*");
static std::regex SCREAMING_SNAKE_CASE_REGEX("[A-Z]+(_([A-Z]*))*");
static std::regex KEBAB_CASE_REGEX("[a-z]+(-([a-z]*))*");
static std::regex CAMEL_CASE_REGEX("[a-z]+([A-Z][a-z]*)*");
static std::regex PASCAL_CASE_REGEX("([A-Z][a-z]*)+");

static std::regex SCALE_CASE_REGEX("([A-Z]|[a-z])([A-Z]|[a-z]|[0-9]|_|-)*");

namespace Nectere
{
	namespace Script
	{
		CodingStyle CodingStyle::ms_CodingStyle;
		bool CodingStyle::ms_DoCodingStyle = true;

		static bool is_snake_case(const std::string &str) { return std::regex_match(str, SNAKE_CASE_REGEX); }
		static bool is_screaming_snake_case(const std::string &str) { return std::regex_match(str, SCREAMING_SNAKE_CASE_REGEX); }
		static bool is_kebab_case(const std::string &str) { return std::regex_match(str, KEBAB_CASE_REGEX); }
		static bool is_camel_case(const std::string &str) { return std::regex_match(str, CAMEL_CASE_REGEX); }
		static bool is_pascal_case(const std::string &str) { return std::regex_match(str, PASCAL_CASE_REGEX); }

		bool CodingStyle::IsNameCompliant(const NameType &nameType, const std::string &nameToCheck)
		{
			switch (nameType)
			{
			case NameType::Variable:
			{
				if (m_VariableChecker)
					return m_VariableChecker(nameToCheck);
				return true;
			}
			case NameType::Global:
			{
				if (m_GlobalChecker)
					return m_GlobalChecker(nameToCheck);
				return true;
			}
			case NameType::DeclaredObject:
			{
				if (m_DeclaredObjectChecker)
					return m_DeclaredObjectChecker(nameToCheck);
				return true;
			}
			case NameType::Function:
			{
				if (m_FunctionChecker)
					return m_FunctionChecker(nameToCheck);
				return true;
			}
			case NameType::Count:
			case NameType::Invalid:
				return false;
			}
			return false;
		}

		bool CodingStyle::SetChecker(const NameType &nameType, bool (*newChecker)(const std::string &))
		{
			if (!newChecker)
				return false;
			switch (nameType)
			{
			case NameType::Variable:
			{
				m_VariableChecker = newChecker;
				return true;
			}
			case NameType::Global:
			{
				m_GlobalChecker = newChecker;
				return true;
			}
			case NameType::DeclaredObject:
			{
				m_DeclaredObjectChecker = newChecker;
				return true;
			}
			case NameType::Function:
			{
				m_FunctionChecker = newChecker;
				return true;
			}
			case NameType::Count:
			case NameType::Invalid:
				return false;
			}
			return false;
		}

		void CodingStyle::Init()
		{
			ms_DoCodingStyle = Nectere::Configuration::Is("ScriptEngine.DoCodingStyle", true);
		}

		void CodingStyle::SetDefaultCodingStyle()
		{
			SetCheckerFor(NameType::Variable, CasePreset::CamelCase);
			SetCheckerFor(NameType::DeclaredObject, CasePreset::PascalCase);
			SetCheckerFor(NameType::Function, CasePreset::SnakeCase);
			SetCheckerFor(NameType::Global, CasePreset::ScreamingSnakeCase);
		}

		const char *CodingStyle::ErrorNameOf(const NameType &nameType)
		{
			switch (nameType)
			{
			case NameType::Variable:
				return VARIABLE_ERROR_NAME;
			case NameType::Global:
				return GLOBAL_ERROR_NAME;
			case NameType::DeclaredObject:
				return DECLARED_OBJECT_ERROR_NAME;
			case NameType::Function:
				return FUNCTION_ERROR_NAME;
			case NameType::Count:
			case NameType::Invalid:
				return "";
			}
			return "";
		}

		bool CodingStyle::SetCheckerFor(const NameType &nameType, const CasePreset &casePreset)
		{
			switch (casePreset)
			{
			case CasePreset::CamelCase:
				return ms_CodingStyle.SetChecker(nameType, &is_camel_case);
			case CasePreset::PascalCase:
				return ms_CodingStyle.SetChecker(nameType, &is_pascal_case);
			case CasePreset::SnakeCase:
				return ms_CodingStyle.SetChecker(nameType, &is_snake_case);
			case CasePreset::ScreamingSnakeCase:
				return ms_CodingStyle.SetChecker(nameType, &is_screaming_snake_case);
			case CasePreset::KebabCase:
				return ms_CodingStyle.SetChecker(nameType, &is_kebab_case);
			case CasePreset::Count:
			case CasePreset::Invalid:
				return false;
			}
			return false;
		}

		bool CodingStyle::SetCheckerFor(const NameType &nameType, bool (*newChecker)(const std::string &)) { return ms_CodingStyle.SetChecker(nameType, newChecker); }

		bool CodingStyle::CheckName(const NameType &nameType, const std::string &nameToCheck)
		{
			if (!std::regex_match(nameToCheck, SCALE_CASE_REGEX))
			{
				LOG_SCRIPT_PARSING(LogType::Error, ErrorNameOf(nameType), " name \"", nameToCheck, "\": A name should start with an alphabetical character and be composed of only alphanumerical character or - or _");
				return false;
			}
			if (!ms_DoCodingStyle)
				return true;
			if (!ms_CodingStyle.IsNameCompliant(nameType, nameToCheck))
			{
				LOG_SCRIPT_PARSING(LogType::Error, ErrorNameOf(nameType), " name \"", nameToCheck, "\": Does not comply to coding style");
				return false;
			}
			return true;
		}
	}
}