#include "Parameters/VerboseParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		bool VerboseParameter::DefaultValue() const
		{
			return false;
// 			#ifndef DEBUG
// 				return false;
// 			#else
// 				return true;
// 			#endif
		}

		bool VerboseParameter::Parse() const { return true; }

		bool VerboseParameter::Parse(const std::string &value) const { return (value == "on"); }

		std::string VerboseParameter::GetDescription() const { return "-v --verbose[=on/off]\t\tEnable or disable verbose."; }

		VerboseParameter::VerboseParameter() : Configuration::ATypedParameter<bool>("Verbose")
		{
			AddRestriction("verbose", PARG_DOUBLE_DASH_RESTRICTION, PARG_OPTIONNAL_VALUE);
			AddRestriction("v", PARG_SIMPLE_DASH_RESTRICTION, PARG_NEED_NO_VALUE);
		}
	}
}