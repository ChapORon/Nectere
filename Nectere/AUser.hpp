#pragma once

#include "Event.hpp"

namespace Nectere
{
	class UserManager;
	class AUser
	{
		friend class UserManager;
	private:
		uint16_t m_ID{ 0 };
		UserManager *m_UserManager{ nullptr };

	private:
		void InternalInit(uint16_t, UserManager *);

	protected:
		void Send(const Event &);

	public:
		uint16_t GetID() const { return m_ID; }
		virtual void Receive(const Event &) = 0;
		virtual void OnInit() {}
		virtual void Update() {}
	};
}