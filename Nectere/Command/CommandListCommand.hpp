#pragma once

#include "ACommand.hpp"

namespace Nectere
{
	namespace Command
	{
		class CommandListCommand : public ACommand
		{
		public:
			CommandListCommand();
			bool IsValid(const std::string &) const override;
			void Treat(uint16_t, uint16_t, const std::string &) override;
		};
	}
}