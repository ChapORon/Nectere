#include "Network/NetworkPortParameter.hpp"

namespace Nectere
{
	namespace Network
	{
		int NetworkPortParameter::DefaultValue() const { return 4242; }

		int NetworkPortParameter::Parse(const std::string &value) const { return std::stoi(value); }

		std::string NetworkPortParameter::GetDescription() const { return "-p [port] --port=[port]\t\tSet the port the server will listen to."; }

		NetworkPortParameter::NetworkPortParameter() : Configuration::ATypedParameter<int>("Network.Port", "network.port")
		{
			AddRestriction("port", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_VALUE);
			AddRestriction("p", PARG_SIMPLE_DASH_RESTRICTION, PARG_NEED_VALUE);
		}
	}
}