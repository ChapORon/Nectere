#pragma once

#include "Configuration.hpp"

namespace Nectere
{
	namespace Network
	{
		class NetworkWhitelistParameter final : public Configuration::ATypedParameter<std::string>
		{
		private:
			std::string DefaultValue() const override;
			std::string Parse(const std::string &value) const override;
			std::string GetDescription() const override;

		public:
			NetworkWhitelistParameter();
		};
	}
}