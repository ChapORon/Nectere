#pragma once

#include <string>
#include <unordered_set>

namespace Nectere
{
	class Canal final
	{
	private:
		bool m_IsMarkForDelete{ false };
		int m_Limit{ -1 };
		std::string m_Name;
		std::string m_Password;
		std::unordered_set<uint16_t> m_Users;

	public:
		Canal() = default;
		Canal(const std::string &, const std::string &, int);
		inline const std::string &GetName() const { return m_Name; }
		inline const std::string &GetPassword() const { return m_Password; }
		inline const std::unordered_set<uint16_t> &GetUsers() const { return m_Users; }
		inline bool IsEmpty() { return m_Users.empty(); }
		inline void MarkForDelete() { m_IsMarkForDelete = true; }
		inline bool IsMarkForDelete() { return m_IsMarkForDelete; }
		inline void AddUser(uint16_t userId) { m_Users.emplace(userId); }
		inline bool IsFull() { return m_Limit != -1 && m_Users.size() == m_Limit; }
		void RemoveUser(uint16_t);
	};
}