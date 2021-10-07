#pragma once

#include "Network/IEventReceiver.hpp"

namespace Nectere
{
	class ApplicationManager;
	namespace Network
	{
		class ApplicationEventReceiver: public IEventReceiver
		{
		private:
			ApplicationManager *m_ApplicationManager;

		public:
			ApplicationEventReceiver(ApplicationManager *);
			void OnReceive(uint16_t, const Event &) override;
		};
	}
}