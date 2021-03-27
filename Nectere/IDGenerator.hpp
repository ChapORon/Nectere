#pragma once

#include <mutex>
#include <queue>

namespace Nectere
{
	class IDGenerator
	{
	private:
		uint16_t m_ID;
		std::queue<uint16_t> m_FreedID;
		std::mutex m_Mutex;

	public:
		IDGenerator();
		uint16_t GenerateID();
		void FreeID(uint16_t);
	};
}