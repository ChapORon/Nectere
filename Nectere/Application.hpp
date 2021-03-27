#pragma once

#include "ACommand.hpp"
#include "Event.hpp"
#include "UIDVector.hpp"

namespace Nectere
{
	class Application
	{
	private:
		uint16_t m_ID;
		std::string m_Name;
		UIDVector<ACommand> m_Commands;

	public:
		Application(uint16_t, const std::string &);
		uint16_t GetID() const { return m_ID; }
		const std::string &GetName() const { return m_Name; };
		void AddCommand(const std::shared_ptr<ACommand> &);
		bool IsEventAllowed(const Event &);
		void Treat(uint16_t, const Event &);
	};
}