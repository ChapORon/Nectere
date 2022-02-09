#pragma once

#include <functional>
#include "Concurrency/Task.hpp"
#include "Logger.hpp"

namespace Nectere
{
    class Manager;
    namespace Concurrency
    {
        class AThreadSystem
        {
            friend class Nectere::Manager;

        protected:
            const Logger *m_Logger = nullptr;

		private:
            void SetLogger(const Logger *logger) { m_Logger = logger; }

		protected:
			virtual void AddTask(Task *) = 0;

			template <class... t_Args>
			void Log(LogType logType, t_Args &&... args) const
			{
				if (m_Logger)
					m_Logger->Log(logType, std::forward<t_Args>(args)...);
			}

        public:
			template <typename t_Object>
			void AddTask(t_Object *obj, TaskResult(t_Object:: *fct)()) { AddTask(new FunctorTask<t_Object>(obj, fct)); }
			template <typename t_Task, typename ...t_Arg>
			void AddTask(t_Arg&&... args) { AddTask(new t_Task(std::forward<t_Arg>(args)...)); }
			void AddTask(const std::function<TaskResult()> &taskToAdd) { AddTask(new CallableTask(taskToAdd)); }

            virtual void Start() = 0;
            virtual void SetOnStopCallback(const std::function<void()> &onStopCallback) = 0;
            virtual void Stop() = 0;
            virtual void Await() = 0;

            virtual ~AThreadSystem() = default;
        };
    }
}