#include "Parameters/ScriptEngineSmartBuildParameter.hpp"

namespace Nectere
{
	namespace Parameters
	{
		bool ScriptEngineSmartBuildParameter::DefaultValue() const
		{
			return true;
		}

		bool ScriptEngineSmartBuildParameter::Parse() const { return true; }

		bool ScriptEngineSmartBuildParameter::Parse(const std::string &value) const { return (value == "on"); }

		std::string ScriptEngineSmartBuildParameter::GetDescription() const { return "--smart-build=on/off\t\tEnable or disable smart build in Nectere script engine."; }

		ScriptEngineSmartBuildParameter::ScriptEngineSmartBuildParameter() : Configuration::ATypedParameter<bool>("ScriptEngine.DoSmartBuild", "script.smart_build")
		{
			AddRestriction("smart-build", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_VALUE);
		}
	}
}