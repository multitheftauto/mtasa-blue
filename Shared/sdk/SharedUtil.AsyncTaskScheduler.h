#pragma once
#include <queue>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>

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

        template <typename ResultType>
        struct STask : public SBaseTask
        {
            using TaskFunction_t = std::function<ResultType()>;
            using ReadyFunction_t = std::function<void(const ResultType&)>;

            TaskFunction_t  m_TaskFunction;
            ReadyFunction_t m_ReadyFunction;
            ResultType      m_Result;

            STask(const TaskFunction_t& taskFunc, const ReadyFunction_t& readyFunc) : m_TaskFunction(taskFunc), m_ReadyFunction(readyFunc) {}

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
        template <typename ResultType>
        void PushTask(const std::function<ResultType()>& taskFunc, const std::function<void(const ResultType&)>& readyFunc)
        {
            std::unique_ptr<SBaseTask> pTask{new STask<ResultType>{taskFunc, readyFunc}};

            std::lock_guard<std::mutex> lock{m_TasksMutex};
            m_Tasks.push(std::move(pTask));
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
