#pragma once

#include "ACommand.hpp"

namespace Nectere
{
	class UserManager;
	namespace Command
	{
		class AuthCommand : public ACommand
		{
		private:
			Ptr<UserManager> m_UserManager;

		public:
			AuthCommand(const Ptr<UserManager> &);
			bool IsValid(const std::string &) const override { return true; }
			void Treat(uint16_t, uint16_t, const std::string &) override;
		};
	}
}