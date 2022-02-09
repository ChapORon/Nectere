#pragma once

#include "ACanalCommand.hpp"

namespace Nectere
{
	namespace Command
	{
		class ListCanalCommand : public ACanalCommand
		{
		public:
			ListCanalCommand(const Ptr<CanalManager> &);
			void Treat(uint16_t, uint16_t, const std::string &) override;
		};
	}
}