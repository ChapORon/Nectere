#include "Task.hpp"

namespace Nectere
{
	Task::Task():
		m_UpdateOnStart(false),
		m_InternalStep(ETaskStep::None),
		m_NeedCancel(false),
		m_Finished(false) {}

	Task::Task(bool updateOnStart):
		m_UpdateOnStart(updateOnStart),
		m_InternalStep(ETaskStep::None),
		m_NeedCancel(false),
		m_Finished(false) {}

	TaskResult Task::Update()
	{
		ComputeTaskState();
		switch (m_InternalStep)
		{
		case ETaskStep::Cancel:
		{
			OnTaskCanceled();
			EndTask();
			return TaskResult::Fail;
		}
		case ETaskStep::Start:
		{
			OnTaskStart();
			if (!m_UpdateOnStart)
				return TaskResult::NeedUpdate;
			[[fallthrough]];
		}
		case ETaskStep::Update:
			return UpdateTask();
		default:
			return TaskResult::Fail;
		}
	}

	void Task::ComputeTaskState()
	{
		if (m_InternalStep != ETaskStep::Finished)
		{
			if (m_NeedCancel.load())
				m_InternalStep = ETaskStep::Cancel;
			else if (m_InternalStep == ETaskStep::None)
				m_InternalStep = ETaskStep::Start;
			else
				m_InternalStep = ETaskStep::Update;
		}
	}

	TaskResult Task::UpdateTask()
	{
		TaskResult result = OnUpdate();
		switch (result)
		{
		case TaskResult::Success:
		{
			OnTaskSuccess();
			EndTask();
			break;
		}
		case TaskResult::Fail:
		{
			OnTaskFail();
			EndTask();
			break;
		}
		case TaskResult::NeedUpdate:
			break;
		}
		return result;
	}

	void Task::EndTask()
	{
		OnTaskEnd();
		m_InternalStep = ETaskStep::Finished;
		m_Finished.store(true);
		m_TaskCondition.notify_all();
	}

	void Task::Await()
	{
		std::unique_lock<std::mutex> lock(m_TaskMutex);
		m_TaskCondition.wait(lock, [=] { return m_Finished.load(); });
	}

	void Task::Cancel()
	{
		m_NeedCancel.store(true);
	}

	Task::~Task()
	{
		if (m_InternalStep != ETaskStep::Finished && m_InternalStep != ETaskStep::None)
		{
			OnTaskCanceled();
			EndTask();
		}
	}
}