#include "Task.hpp"

namespace Nectere
{
	Task::Task():
		m_UpdateOnStart(false),
		m_InternalStep(ETaskStep::NONE),
		m_NeedCancel(false),
		m_Finished(false) {}

	Task::Task(bool updateOnStart):
		m_UpdateOnStart(updateOnStart),
		m_InternalStep(ETaskStep::NONE),
		m_NeedCancel(false),
		m_Finished(false) {}

	TaskResult Task::Update()
	{
		ComputeTaskState();
		switch (m_InternalStep)
		{
		case ETaskStep::CANCEL:
		{
			OnTaskCanceled();
			EndTask();
			return TaskResult::FAIL;
		}
		case ETaskStep::START:
		{
			OnTaskStart();
			if (!m_UpdateOnStart)
				return TaskResult::NEED_UPDATE;
			[[fallthrough]];
		}
		case ETaskStep::UPDATE:
			return UpdateTask();
		default:
			return TaskResult::FAIL;
		}
	}

	void Task::ComputeTaskState()
	{
		if (m_InternalStep != ETaskStep::FINISHED)
		{
			if (m_NeedCancel.load())
				m_InternalStep = ETaskStep::CANCEL;
			else if (m_InternalStep == ETaskStep::NONE)
				m_InternalStep = ETaskStep::START;
			else
				m_InternalStep = ETaskStep::UPDATE;
		}
	}

	TaskResult Task::UpdateTask()
	{
		TaskResult result = OnUpdate();
		switch (result)
		{
		case TaskResult::SUCCESS:
		{
			OnTaskSuccess();
			EndTask();
			break;
		}
		case TaskResult::FAIL:
		{
			OnTaskFail();
			EndTask();
			break;
		}
		case TaskResult::NEED_UPDATE:
			break;
		}
		return result;
	}

	void Task::EndTask()
	{
		OnTaskEnd();
		m_InternalStep = ETaskStep::FINISHED;
		m_Finished.store(true);
		m_TaskCondition.notify_all();
	}

	void Task::Await()
	{
		m_TaskCondition.wait(std::unique_lock<std::mutex>(m_TaskMutex), [=] { return m_Finished.load(); });
	}

	void Task::Cancel()
	{
		m_NeedCancel.store(true);
	}

	Task::~Task()
	{
		if (m_InternalStep != ETaskStep::FINISHED && m_InternalStep != ETaskStep::NONE)
		{
			OnTaskCanceled();
			EndTask();
		}
	}
}