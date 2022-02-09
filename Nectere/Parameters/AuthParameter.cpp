#include "Parameters/AuthParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		std::string AuthParameter::DefaultValue() const
		{
			return "";
		}

		std::string AuthParameter::Parse(const std::string &value) const
		{
			return value;
		}

		std::string AuthParameter::GetDescription() const { return "--auth=[auth]\t\tSpecify the auth key to manage server. If empty, anyone will be able to auth through the auth command."; }

		AuthParameter::AuthParameter(): Configuration::ATypedParameter<std::string>("AuthKey", "auth")
		{
			AddRestriction("auth", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_VALUE);
		}
	}
}