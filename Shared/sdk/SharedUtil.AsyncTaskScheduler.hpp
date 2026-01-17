#include "SharedUtil.AsyncTaskScheduler.h"

// Workaround MultiplayerSA including this header for whatever reason..
#ifdef HAS_ASYNC_TASK_SCHED

namespace SharedUtil
{
    CAsyncTaskScheduler::CAsyncTaskScheduler(std::size_t numWorkers)
    {
        for (std::size_t i = 0; i < numWorkers; ++i)
        {
            m_Workers.emplace_back(&CAsyncTaskScheduler::DoWork, this);
        }
    }

    CAsyncTaskScheduler::~CAsyncTaskScheduler()
    {
        m_Running = false;

        // Wait for all threads to end
        for (auto& thread : m_Workers)
        {
            if (thread.joinable())
                thread.join();
        }
    }

    void CAsyncTaskScheduler::CollectResults()
    {
        std::lock_guard<std::mutex> lock{m_TaskResultsMutex};

        for (auto& pTask : m_TaskResults)
        {
            pTask->ProcessResult();
        }
        m_TaskResults.clear();
    }

    void CAsyncTaskScheduler::DoWork()
    {
        while (m_Running)
        {
            m_TasksMutex.lock();

            // Sleep a bit if there are no tasks
            if (m_Tasks.empty())
            {
                m_TasksMutex.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(4));
                continue;
            }

            // Get task and remove from front
            std::unique_ptr<SBaseTask> pTask = std::move(m_Tasks.front());
            m_Tasks.pop();

            m_TasksMutex.unlock();

            // Execute time-consuming task
            pTask->Execute();

            // Put into result queue
            {
                std::lock_guard<std::mutex> lock{m_TaskResultsMutex};

                m_TaskResults.push_back(std::move(pTask));
            }
        }
    }
}            // namespace SharedUtil
#endif
