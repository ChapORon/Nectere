#pragma once

#include <thread>
#include "Task.hpp"

namespace Nectere
{
	class Thread final
	{
	private:
		int m_ID;
		std::mutex m_ThreadMutex;
		std::atomic_bool m_Running;
		std::atomic_bool m_Stopping;
		std::atomic_bool m_Stopped;
		std::function<TaskResult(int)> m_Function;
		std::thread m_Thread;
		std::condition_variable m_ThreadCondition;

	private:
		void Loop();
		void Run();

	public:
		Thread(int);
		Thread(int, const std::function<TaskResult(int)> &);
		void SetFunction(const std::function<TaskResult(int)> &);
		void Start();
		void Stop();
	};
}