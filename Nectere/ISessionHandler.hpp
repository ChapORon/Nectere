#pragma once

#include "Event.hpp"

namespace Nectere
{
	class ISessionHandler
	{
	public:
		virtual void OnReceive(uint16_t sessionId, const Event &message) = 0;
		virtual void OnWrite(uint16_t sessionId) = 0;
		virtual void CloseSession(uint16_t sessionId) = 0;
	};
}