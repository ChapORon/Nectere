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

		private:
			static bool ms_DoCodingStyle;

		private:
			static bool IsNameCompliant(const NameType &, const std::string &);
			static const char *ErrorNameOf(const NameType &);

		public:
			static void Init();
			static bool CheckName(const NameType &, const std::string &);
		};
	}
}