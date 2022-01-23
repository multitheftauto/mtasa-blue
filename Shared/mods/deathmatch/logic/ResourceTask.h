#pragma once
#include "SharedUtil.Defines.h"
#include "../../../sdk/SharedUtil.AsyncTaskScheduler.h" // TODO: Figure out better way to do this

//
// A task associated with a resource
// On resouce stop all associated tasks are cancelled
// No functions in this class are thread safe unless marked otherwise
//

struct BaseResourceTask : CAsyncTaskScheduler::SBaseTask
{
    BaseResourceTask(CLuaMain& creator) :
         m_Creator{creator}
    {
        // No need to notify creator, as it has just created us!
        dassert(IsMainThread());
    }

    virtual ~BaseResourceTask() { 
        assert(IsMainThread()); // Tasks should only be destructed after processed on the main thread

        if (!WasCancelled()) { // Important check, because otherwise `Creator` might be a dangling reference
            NotifyCreatorWeFinished();
        }

        // Right now we only have an active list (in which we should only be if we weren't cancelled)
        // But let's put it here, in case anyone ever adds a second list or something...
        RemoveFromList();
    }

    auto Next()   const { return m_Next; }
    auto Prev()   const { return m_Prev; }
    bool WasCancelled() { return m_Cancelled; } // Thread-safe

    // Cancel task - Should only be called from the main thread
    // Removes task from list, and notifies creator (So it can adjust it's list head)
    void Cancel() {
        dassert(IsMainThread());                               // Avoid race-conditions
        dassert(!WasCancelled());                              // Otherwise `NotifyCreatorOfCancellation` will be called more than once
        
        m_Cancelled = true;
        NotifyCreatorOfCancellation();
        RemoveFromList(); // Maybe the creator should call this instead when notified?
    }

    void AddToList(BaseResourceTask* head) {
        dassert(IsMainThread());         // Otherwise we might have race-conditions
        dassert(!m_Next && !m_Prev);     // Make sure we aren't in a list already (Note: It's possible we're the head though, we can't check that here..)
        dassert(!head || !head->m_Prev); // Head shouldn't have no previous link

        m_Next = head;
        if (head) // There may be no head, in which case we'll become the head
            head->m_Prev = this;
    }

private:
    void NotifyCreatorOfCancellation();
    void NotifyCreatorWeFinished();

    // Not thread safe
    // Caller should make sure Creator's list head is adjusted
    // as we might be the head itself.
    void RemoveFromList() {
        dassert(IsMainThread());                    // Otherwise we might have race-conditions
        dassert(!m_Next || m_Next->m_Prev == this); // Sanity checks to make sure links aren't messed up
        dassert(!m_Prev || m_Prev->m_Next == this); 

        if (m_Next)
            m_Next->m_Prev = m_Prev;
        if (m_Prev)
            m_Prev->m_Next = m_Next;
    }

private:
    // If this is set the `Task` function won't be called, and it will it will be discared on the main-thread.
    // If the task has already been processed the `Ready` function won't be called
    std::atomic<bool> m_Cancelled{};

    // Resource this task was created by
    // There's a possibility we outlive it, but thats fine, 
    // because at that point `m_Cancelled` should always be set
    class CLuaMain& m_Creator;

    // Links to all ACTIVE tasks with the same `creator` as us
    BaseResourceTask *m_Prev{}, *m_Next{};
};

template <typename TaskFn, typename ReadyFn>
struct ResourceTask : BaseResourceTask
{
    using Result = std::invoke_result_t<TaskFn>;

    TaskFn  m_TaskFunction;
    ReadyFn m_ReadyFunction;
    Result  m_Result;

    ResourceTask(CLuaMain& creator, TaskFn&& taskfn, ReadyFn&& readyfn) :
        BaseResourceTask{ creator },
        m_TaskFunction{ std::move(taskfn) },
        m_ReadyFunction{ std::move(readyfn) }
    {
    }

    // Thread-safe (Should only be called from a worker thread)
    void Execute() override {
        dassert(!IsMainThread());

        if (!WasCancelled()) {
            m_Result = std::move(m_TaskFunction());
        }
    }

    // Thread-safe (But should only ever be called from the main thread)
    void ProcessResult() override {
        dassert(IsMainThread());

        if (!WasCancelled()) {
            m_ReadyFunction(std::move(m_Result));
        }
    }
};
