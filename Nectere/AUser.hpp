#pragma once

#include "nectere_export.h"
#include "Event.hpp"

namespace Nectere
{
	class UserManager;
	class AUser
	{
		friend class UserManager;
	private:
		bool m_Authenticated;
		uint16_t m_ID{ 0 };
		UserManager *m_UserManager{ nullptr };

	private:
		void InternalInit(uint16_t, UserManager *);
		void Authenticate() { m_Authenticated = true; }

	protected:
		NECTERE_EXPORT void Send(const Event &);
		NECTERE_EXPORT void Close();
		NECTERE_EXPORT void MarkForDelete();

	public:
		inline uint16_t GetID() const { return m_ID; }
		NECTERE_EXPORT bool IsAuthenticated() { return m_Authenticated; }
		NECTERE_EXPORT virtual void Receive(const Event &) {};
		NECTERE_EXPORT virtual void OnInit() {}
		NECTERE_EXPORT virtual void Update() {}
		NECTERE_EXPORT virtual void OnClose() {}
	};
}