#pragma once

#include "Concurrency/AThreadSystem.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"

namespace Nectere
{
	class UserManager;
	class Manager
	{
	private:
		Configuration m_Configuration;
		Concurrency::AThreadSystem *m_ThreadSystem = nullptr;
		Logger *m_Logger = nullptr;
		UserManager *m_UserManager = nullptr;

	public:
		template <typename t_ThreadSystem, typename ...t_Arg>
		bool SetThreadSystem(t_Arg&&... args)
		{
			if (m_ThreadSystem)
			{
				m_ThreadSystem->Stop();
				m_ThreadSystem->Await();
				delete(m_ThreadSystem);
			}
			m_ThreadSystem = new t_ThreadSystem(std::forward<t_Arg>(args)...);
			return m_ThreadSystem != nullptr;
		}

		Configuration &GetConfiguration() { return m_Configuration; }
		Concurrency::AThreadSystem *GetThreadSystem() const { return m_ThreadSystem; }
		const Logger *GetLogger() const { return m_Logger; }
		UserManager *GetUserManager() const { return m_UserManager; }

		bool Start(int, char **);
		void Await();
		void Stop();
		~Manager();
	};
}