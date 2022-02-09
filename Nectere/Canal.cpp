#include "Canal.hpp"

namespace Nectere
{
	Canal::Canal(const std::string &name, const std::string &password, int limit) : m_Limit(limit), m_Name(name), m_Password(password) {}

	void Canal::RemoveUser(uint16_t userId)
	{
		auto it = m_Users.find(userId);
		if (it != m_Users.end())
			m_Users.erase(it);
	}
}