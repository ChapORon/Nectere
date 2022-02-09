#pragma once

#include "ACommand.hpp"
#include "Ptr.hpp"

namespace Nectere
{
	namespace Concurrency
	{
		class AThreadSystem;
	}

	namespace Command
	{
		class StopCommand: public ACommand
		{
		private:
			Ptr<Concurrency::AThreadSystem> m_ThreadSystem;

		public:
			StopCommand(const Ptr<Concurrency::AThreadSystem> &);
			bool IsValid(const std::string &) const override;
			void Treat(uint16_t, uint16_t, const std::string &) override;
		};
	}
}