#pragma once

#include <functional>
#include <memory>
#include <queue>
#include "Concurrency/AThreadSystem.hpp"
#include "Concurrency/Task.hpp"
#include "Concurrency/Thread.hpp"

namespace Nectere
{
    namespace Concurrency
    {
        class ThreadSystem: public AThreadSystem
        {
        private:
            enum class ThreadType: short
            {
                TaskThread,
                SchedulerThread
            };

        private:
            std::thread::id m_CreatorThreadID;
            std::atomic_bool m_Running;
            std::queue<Task *> m_Tasks;
            std::mutex m_TaskMutex;
            std::mutex m_ThreadsMutex;
            std::mutex m_ShedulerMutex;
            std::condition_variable m_TaskCondition;
            std::condition_variable m_ThreadsCondition;
            std::vector<std::pair<Thread *, ThreadType>> m_Threads;
            std::vector<std::pair<size_t, Task *>> m_ScheduledTasks;
            std::function<void()> m_OnStopCallback;

        private:
            TaskResult TaskThreadLoop(int);
            TaskResult ShedulerThreadLoop(int);
            void Clean();
            void AddTask(Task *) override;

        public:
            ThreadSystem();
            void Start() override;
            void SetOnStopCallback(const std::function<void()> &onStopCallback) override { m_OnStopCallback = onStopCallback; }
            void Stop() override;
            void Await() override;
            ~ThreadSystem();
        };
    }
}