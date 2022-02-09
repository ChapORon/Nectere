#include "Concurrency/Thread.hpp"

#include "Logger.hpp"

namespace Nectere
{
    namespace Concurrency
    {
        Thread::Thread(const Logger *logger, int id) : m_ID(id), m_Logger(logger), m_Running(false), m_Stopping(false), m_Stopped(false), m_Function([](int) {return TaskResult::NeedUpdate; }) {}
        Thread::Thread(const Logger *logger, int id, const std::function<TaskResult(int)> &fct): m_ID(id), m_Logger(logger), m_Running(false), m_Stopping(false), m_Stopped(false), m_Function(fct) {}

        void Thread::Loop()
        {
            while (m_Running.load())
            {
                switch (m_Function(m_ID))
                {
                case TaskResult::Fail:
                case TaskResult::Success:
                    return;
                }
                std::this_thread::yield();
            }
        }

        void Thread::Run()
        {
            Loop();
            Log(LogType::Standard, "[Thread ", m_ID, "] Thread loop has stopped");
            m_Stopped.store(true);
            m_ThreadCondition.notify_all();
        }

        void Thread::SetFunction(const std::function<TaskResult(int)> &fct)
        {
            m_Function = fct;
        }

        void Thread::Start()
        {
            if (!m_Running.load() && !m_Stopping.load())
            {
                m_Running.store(true);
                Log(LogType::Standard, "[Thread ", m_ID, "] Starting thread");
                m_Thread = std::thread(std::bind(&Thread::Run, this));
            }
        }

        void Thread::Stop()
        {
            if (m_Running.load())
            {
                Log(LogType::Standard, "[Thread ", m_ID, "] Stopping thread");
                m_Stopping.store(true);
                m_Running.store(false);
                std::unique_lock<std::mutex> lock(m_ThreadMutex);
                m_ThreadCondition.wait(lock, [=] { return m_Stopped.load(); });
                try
                {
                    m_Thread.join();
                } catch (const std::exception &e) {}
                m_Stopping.store(false);
            }
        }
    }
}