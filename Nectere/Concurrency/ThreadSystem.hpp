#pragma once

#include <functional>
#include <memory>
#include <queue>
#include "Concurrency/Task.hpp"
#include "Concurrency/TaskResult.hpp"
#include "Concurrency/Thread.hpp"

namespace Nectere
{
    namespace Concurrency
    {
        class ThreadSystem
        {
        private:
            enum class ThreadType: short
            {
                TaskThread,
                SchedulerThread,
                AllocatedThread
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

        private:
            TaskResult TaskThreadLoop(int);
            TaskResult ShedulerThreadLoop(int);
            void Clean();
            void AddTask(Task *);

        public:
            ThreadSystem();
            void AllocateThread(const std::function<TaskResult(int)> &);
            template <typename t_Object>
            void AddTask(t_Object *obj, TaskResult(t_Object:: *fct)()) { AddTask<FunctorTask<t_Object>>(obj, fct); }
            template <typename t_Task, typename ...t_Arg>
            void AddTask(t_Arg&&... args) { AddTask(new t_Task(args...)); }
            void AddTask(const std::function<TaskResult()> &taskToAdd) { AddTask<CallableTask>(taskToAdd); }
            void Start();
            void Stop();
            void Await();
            ~ThreadSystem();
        };
    }
}