#pragma once

#include <version>

// Workaround MultiplayerSA including this header for whatever reason..
#ifdef __cpp_lib_is_invocable
#define HAS_ASYNC_TASK_SCHED

#include <queue>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <type_traits>

namespace SharedUtil
{
    ///////////////////////////////////////////////////////////////
    //
    // CAsyncTaskScheduler class
    //
    // Asynchronously executes tasks in secondary worker threads
    // and returns the result back to the main thread
    //
    ///////////////////////////////////////////////////////////////
    class CAsyncTaskScheduler
    {
        struct SBaseTask
        {
            virtual ~SBaseTask() {}
            virtual void Execute() = 0;
            virtual void ProcessResult() = 0;
        };

        template <typename TaskFn, typename ReadyFn>
        struct STask : public SBaseTask
        {
            using Result = std::invoke_result_t<TaskFn>;

            TaskFn  m_TaskFunction;
            ReadyFn m_ReadyFunction;
            Result  m_Result;

            STask(TaskFn&& task, ReadyFn&& ready) : m_TaskFunction(std::move(task)), m_ReadyFunction(std::move(ready)) {}

            void Execute() override { m_Result = std::move(m_TaskFunction()); }

            void ProcessResult() override { m_ReadyFunction(m_Result); }
        };

    public:
        //
        // Creates a new async task scheduler
        // with a fixed number of worker threads
        //
        CAsyncTaskScheduler(std::size_t numWorkers);

        //
        // Ends all worker threads (waits for the last task to finish)
        //
        ~CAsyncTaskScheduler();

        //
        // Pushes a new task for execution once a worker is free
        // (Template Parameter) ResultType: The type of the result
        //
        // taskFunc: Time-consuming function that is executed on the secondary thread (be aware of thread safety!)
        // readyFunc: Function that is called once the result is ready (called on the main thread)
        //
        template <typename TaskFn, typename ReadyFn>
        void PushTask(TaskFn&& task, ReadyFn&& ready)
        {
            std::unique_ptr<SBaseTask> pTask{new STask{std::move(task), std::move(ready)}};

            std::scoped_lock<std::mutex> lock{m_TasksMutex};
            m_Tasks.emplace(std::move(pTask));
        }

        //
        // Collects (polls) the results of the finished tasks
        // and invokes its ready-functions on the main thread
        // THIS FUNCTION MUST BE CALLED ON THE MAIN THREAD
        //
        void CollectResults();

    protected:
        void DoWork();

    private:
        std::vector<std::thread> m_Workers;
        bool                     m_Running = true;

        std::queue<std::unique_ptr<SBaseTask>> m_Tasks;
        std::mutex                             m_TasksMutex;

        std::vector<std::unique_ptr<SBaseTask>> m_TaskResults;
        std::mutex                              m_TaskResultsMutex;
    };
}            // namespace SharedUtil
#endif
