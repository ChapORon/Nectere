#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include "TaskResult.hpp"

namespace Nectere
{
	class Task
	{
	private:
		enum class ETaskStep
		{
			None,
			Start,
			Update,
			Cancel,
			Finished
		};

	private:
		bool m_UpdateOnStart;
		ETaskStep m_InternalStep;
		std::atomic_bool m_NeedCancel;
		std::atomic_bool m_Finished;
		std::mutex m_TaskMutex;
		std::condition_variable m_TaskCondition;

	private:
		virtual TaskResult OnUpdate() = 0;
		virtual void OnTaskStart() {}
		virtual void OnTaskEnd() {}
		virtual void OnTaskFail() {}
		virtual void OnTaskSuccess() {}
		virtual void OnTaskCanceled() {}
		void ComputeTaskState();
		TaskResult UpdateTask();
		void EndTask();

	public:
		Task();
		Task(bool);
		void Cancel();
		TaskResult Update();
		void Await();
		~Task();
	};

	class CallableTask final : public Task
	{
	private:
		std::function<TaskResult()> m_Function;

	public:
		CallableTask(const std::function<TaskResult()> &fct) : Task(), m_Function(fct) {}
		TaskResult OnUpdate() override { return m_Function(); }
	};

	template <typename t_Object>
	class FunctorTask final : public Task
	{
	private:
		t_Object *m_Obj;
		TaskResult(t_Object:: *m_Function)();

	public:
		FunctorTask(t_Object *obj, TaskResult(t_Object::*fct)()) : Task(), m_Obj(obj), m_Function(fct) {}
		TaskResult OnUpdate() override
		{
			if (m_Obj != nullptr && m_Function != nullptr)
				return (m_Obj->*m_Function)();
			return TaskResult::Fail;
		}
	};
}