#include "Parameters/LogPathParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		std::string LogPathParameter::DefaultValue() const
		{
			return "./log/";
		}

		std::string LogPathParameter::Parse(const std::string &value) const
		{
			m_LogInFile->ForceSet(true);
			return value;
		}

		std::string LogPathParameter::GetDescription() const { return "--log-path=[path]\t\tSpecify where to output log file. Setting this will automatically enable log file."; }

		LogPathParameter::LogPathParameter(const std::shared_ptr<LogInFileParameter> &logInFile):
			Configuration::ATypedParameter<std::string>("LogPath", "log.filepath"), m_LogInFile(logInFile)
		{
			AddRestriction("log-path", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_VALUE);
		}
	}
}