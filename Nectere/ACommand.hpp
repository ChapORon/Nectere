#pragma once

#include <string>

namespace Nectere
{
	class ACommand
	{
	private:
		uint16_t m_CommandID;

	public:
		ACommand(uint16_t);
		uint16_t GetID() const { return m_CommandID; }
		virtual bool IsValid(const std::string &) const = 0;
		virtual void Treat(uint16_t, const std::string &) = 0;
	};
}