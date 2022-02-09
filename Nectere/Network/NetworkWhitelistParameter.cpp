#include "Network/NetworkWhitelistParameter.hpp"

namespace Nectere
{
	namespace Network
	{
		std::string NetworkWhitelistParameter::DefaultValue() const { return ""; }

		std::string NetworkWhitelistParameter::Parse(const std::string &value) const { return value; }

		std::string NetworkWhitelistParameter::GetDescription() const { return "--whitelist=[whitelist file]\t\tSet the ip whitelist for the server."; }

		NetworkWhitelistParameter::NetworkWhitelistParameter() : Configuration::ATypedParameter<std::string>("Network.Whitelist", "network.whitelist")
		{
			AddRestriction("whitelist", PARG_DOUBLE_DASH_RESTRICTION, PARG_NEED_VALUE);
		}
	}
}