#pragma once

#include "ACommand.hpp"
#include "Ptr.hpp"

namespace Nectere
{
	class CanalManager;
	namespace Command
	{
		class ACanalCommand : public ACommand
		{
		protected:
			Ptr<CanalManager> m_CanalManager;

		public:
			ACanalCommand(uint16_t, const std::string &, const Ptr<CanalManager> &);
			bool IsValid(const std::string &) const override;
		};
	}
}