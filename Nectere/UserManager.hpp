#pragma once

#include "Event.hpp"
#include "IDGenerator.hpp"
#include "UIDSet.hpp"

namespace Nectere
{
	class AUser;
	class ApplicationManager;
	class UserManager final
	{
		friend class AUser;
		friend class ApplicationManager;
	private:
		ApplicationManager *m_ApplicationManager;
		UIDSet<AUser> m_Users;
		IDGenerator m_IDGenerator;

	private:
		uint16_t InternalAddUser(AUser *);
		void ReceiveEvent(uint16_t, const Event &);
		void SendEvent(uint16_t, const Event &);
		void SendEvent(const std::vector<uint16_t> &, const Event &);

	public:
		UserManager();
		template <typename t_User, typename ...t_Arg>
		uint16_t AddUser(t_Arg&&... args) { return InternalAddUser(new t_User(std::move(args)...)); }
		void RemoveUser(uint16_t);
		ApplicationManager *GetApplicationManager() { return m_ApplicationManager; }
		void Update();
		~UserManager();
	};
}