#pragma once
#include "../../../sdk/SharedUtil.AsyncTaskScheduler.h"
//#include <>

struct BaseResourceTask
{
    // If this is set the `Task` function won't be called, and it will it will be discared on the main-thread.
    // If the task has already been processed the `Ready` function won't be called
    std::atomic<bool> m_Cancelled{};

    // Resource this task was created by
    class CLuaMain* m_Creator{};

    // Links
    BaseResourceTask* m_Prev{}, * m_Next{};

    BaseResourceTask(CLuaMain* creator) :
         m_Creator{creator}
    {
    }

    virtual ~BaseResourceTask();

    bool WasCancelled() { return m_Cancelled; }
    auto Next()   const { return m_Next; }
    auto Prev()   const { return m_Prev; }
    void Cancel()       { m_Cancelled.store(true); }

    void AddToList(BaseResourceTask* head) {
        assert(IsMainThread()); // Otherwise we might have race-conditions

        m_Next = head;
        if (head)
            head->m_Prev = this;

        // Head has no `Prev`, thus no need to adjust it.
    }

    void RemoveFromList() {
        assert(IsMainThread()); // Otherwise we might have race-conditions

        if (m_Next)
            m_Next->m_Prev = m_Prev;
        if (m_Prev)
            m_Prev->m_Next = m_Next;
    }
};

//
// A task connected associated with a resource
// On resouce stop all associated tasks are cancelled
//
template <typename TaskFn, typename ReadyFn>
struct ResourceTask : BaseResourceTask, CAsyncTaskScheduler::STask<TaskFn, ReadyFn>
{
    ResourceTask(CLuaMain* creator, TaskFn&& taskfn, ReadyFn&& readyfn) :
        STask{ std::move(taskfn), std::move(readyfn) },
        BaseResourceTask{ creator }
    {
    }

    void Execute() override {
        if (!WasCancelled()) {
            m_Result = std::move(m_TaskFunction());
        }
    }

    void ProcessResult() override {
        if (!WasCancelled()) {
            m_ReadyFunction(std::move(m_Result));
        }
    }
};
