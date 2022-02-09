#pragma once

#include "ACommand.hpp"

namespace Nectere
{
	namespace Command
	{
		class ReloadAppCommand : public ACommand
		{
		public:
			ReloadAppCommand();
			bool IsValid(const std::string &) const override;
			void Treat(uint16_t, uint16_t, const std::string &) override;
		};
	}
}