#pragma once

#include <thread>
#include "Logger.hpp"
#include "Task.hpp"

namespace Nectere
{
    namespace Concurrency
    {
        class Thread final
        {
        private:
            int m_ID;
            const Logger *m_Logger = nullptr;
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

			template <class... t_Args>
			void Log(LogType logType, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->Log(logType, std::forward<t_Args>(args)...);
			}

			template <class... t_Args>
			void DebugLog(const std::string &function, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->DebugLog("Thread", function, std::forward<t_Args>(args)...);
			}

        public:
            Thread(const Logger *, int);
            Thread(const Logger *, int, const std::function<TaskResult(int)> &);
            void SetFunction(const std::function<TaskResult(int)> &);
            void Start();
            void Stop();
        };
    }
}