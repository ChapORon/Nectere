#pragma once

#include "Configuration.hpp"

namespace Nectere
{
	namespace Network
	{
		class NetworkPortParameter final : public Configuration::ATypedParameter<int>
		{
		private:
			int DefaultValue() const override;
			int Parse(const std::string &value) const override;
			std::string GetDescription() const override;

		public:
			NetworkPortParameter();
		};
	}
}