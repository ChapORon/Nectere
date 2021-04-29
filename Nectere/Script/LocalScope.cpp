#include "Script/LocalScope.hpp"

namespace Nectere
{
	namespace Script
	{
		Variable LocalScope::Get(const std::string &name)
		{
			return m_Locals.at(name).GetMutable();
		}

		void LocalScope::Add(const std::string &name, Variable &&variable)
		{
			m_Locals[name] = variable;
		}
		bool LocalScope::Have(const std::string &name)
		{
			return m_Locals.find(name) != m_Locals.end();
		}
	}
}