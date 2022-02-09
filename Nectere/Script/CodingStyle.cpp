#include "Script/CodingStyle.hpp"

#include <regex>
#include "Configuration.hpp"
#include "Logger.hpp"

static std::regex SNAKE_CASE_REGEX("[a-z]+(_([a-z]*))*");
static std::regex SCREAMING_SNAKE_CASE_REGEX("[A-Z]+(_([A-Z]*))*");
static std::regex KEBAB_CASE_REGEX("[a-z]+(-([a-z]*))*");
static std::regex CAMEL_CASE_REGEX("[a-z]+([A-Z][a-z]*)*");
static std::regex PASCAL_CASE_REGEX("([A-Z][a-z]*)+");

static std::regex NECTERE_SCRIPT_CASE_REGEX("([A-Z]|[a-z])([A-Z]|[a-z]|[0-9]|_|-)*");

namespace Nectere
{
	namespace Script
	{
		bool CodingStyle::ms_DoCodingStyle = true;

		bool CodingStyle::IsNameCompliant(const NameType &nameType, const std::string &nameToCheck)
		{
			bool (*nameChecker)(const std::string &) = nullptr;
			switch (nameType)
			{
			case NameType::Variable:
				return std::regex_match(nameToCheck, CAMEL_CASE_REGEX);
			case NameType::Global:
				return std::regex_match(nameToCheck, SCREAMING_SNAKE_CASE_REGEX);
			case NameType::DeclaredObject:
				return std::regex_match(nameToCheck, PASCAL_CASE_REGEX);
			case NameType::Function:
				return std::regex_match(nameToCheck, SNAKE_CASE_REGEX);
			case NameType::Count:
			case NameType::Invalid:
			default:
				return false;
			}
		}

		void CodingStyle::Init()
		{
			ms_DoCodingStyle = GetConfiguration().Get<bool>("ScriptEngine.DoCodingStyle");
		}

		const char *CodingStyle::ErrorNameOf(const NameType &nameType)
		{
			switch (nameType)
			{
			case NameType::Variable:
				return "Variable";
			case NameType::Global:
				return "Global";
			case NameType::DeclaredObject:
				return "Declared object";
			case NameType::Function:
				return "Function";
			case NameType::Count:
			case NameType::Invalid:
			default:
				return "";
			}
		}

		bool CodingStyle::CheckName(const NameType &nameType, const std::string &nameToCheck)
		{
			if (!std::regex_match(nameToCheck, NECTERE_SCRIPT_CASE_REGEX))
			{
				LOG_SCRIPT_PARSING(LogType::Error, ErrorNameOf(nameType), " name \"", nameToCheck, "\": A name should start with an alphabetical character and be composed of only alphanumerical character or - or _");
				return false;
			}
			if (!ms_DoCodingStyle)
				return true;
			if (!IsNameCompliant(nameType, nameToCheck))
			{
				LOG_SCRIPT_PARSING(LogType::Error, ErrorNameOf(nameType), " name \"", nameToCheck, "\": Does not comply to coding style");
				return false;
			}
			return true;
		}
	}
}