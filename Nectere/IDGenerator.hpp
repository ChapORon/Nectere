#pragma once

#include <mutex>
#include <queue>
#include "nectere_export.h"

namespace Nectere
{
	class IDGenerator
	{
	private:
		uint16_t m_ID;
		std::queue<uint16_t> m_FreedID;
		std::mutex m_Mutex;

	public:
		NECTERE_EXPORT IDGenerator();
		NECTERE_EXPORT IDGenerator(uint16_t);
		NECTERE_EXPORT uint16_t GenerateID();
		NECTERE_EXPORT void FreeID(uint16_t);
	};
}