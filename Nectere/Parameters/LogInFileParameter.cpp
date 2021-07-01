#include "Parameters/LogInFileParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		bool LogInFileParameter::DefaultValue() const
		{
			return true;
		}

		bool LogInFileParameter::Parse() const
		{
			return false;
		}

		std::string LogInFileParameter::GetDescription() const { return "--no-log-file\t\t\tDisable the output of log into a log file."; }

		LogInFileParameter::LogInFileParameter() : Configuration::ATypedParameter<bool>("LogInFile")
		{
			AddRestriction("no-log-file", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		}
	}
}