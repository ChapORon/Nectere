#pragma once

#include "Concurrency/TaskResult.hpp"

namespace Nectere
{
	namespace Concurrency
	{
		class ThreadSystem;
	}
	namespace Network
	{
		class AServer;
	}
	class UserManager;
	class NectereExecutable
	{
	private:
		Concurrency::ThreadSystem *m_ThreadSystem = nullptr;
		Network::AServer *m_Server = nullptr;
		UserManager *m_UserManager = nullptr;

	private:
		void InitConfiguration();
		Concurrency::TaskResult Update();

	public:
		void Start(int, char **);
		void Stop();
		~NectereExecutable();
	};
}