#pragma once

#include "Event.hpp"

namespace Nectere
{
	namespace Network
	{
		class IEventReceiver
		{
		public:
			virtual void OnReceive(uint16_t sessionId, const Event &message) = 0;
		};
	}
}