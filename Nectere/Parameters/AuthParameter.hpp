#pragma once

#include "Configuration.hpp"

namespace Nectere
{
	namespace Parameters
	{
		class AuthParameter final : public Configuration::ATypedParameter<std::string>
		{
		private:
			std::string DefaultValue() const override;
			std::string Parse(const std::string &) const override;
			std::string GetDescription() const override;

		public:
			AuthParameter();
		};
	}
}