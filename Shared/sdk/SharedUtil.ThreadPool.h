/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.ThreadPool.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <thread>
#include <queue>
#include <future>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include "SharedUtil.Misc.h"

namespace SharedUtil
{
    class CThreadPool
    {
    public:
        CThreadPool(std::size_t threads)
        {
            m_vecThreads.reserve(threads);
            for (std::size_t i = 0; i < threads; ++i)
            {
                m_vecThreads.emplace_back([this] {
                    while (true)
                    {
                        std::packaged_task<void(bool)> task;
                        {
                            // Wait until either exit is signalled or a new task arrives
                            std::unique_lock<std::mutex> lock(m_mutex);
                            m_cv.wait(lock, [this] { return m_exit || !m_tasks.empty(); });
                            if (m_exit && m_tasks.empty())
                                return;
                            task = std::move(m_tasks.front());
                            m_tasks.pop();
                        }
                        // Run the task (catch exceptions to prevent thread death)
                        try
                        {
                            task(false);
                        }
                        catch (...)
                        {
                            // Exception is automatically captured by std::packaged_task
                            // and will be re-thrown when future.get() is called.
                            // We must catch here to prevent the worker thread from terminating.
                        }
                    }
                });
            }
        };

        template <typename Func, typename... Args>
        auto enqueue(Func&& f, Args&&... args)
        {
#if __cplusplus < 201703L // C++17
            using ReturnT = typename std::result_of<Func(Args...)>::type;
#else
            using ReturnT = std::invoke_result_t<Func, Args...>;
#endif
            auto  ff = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
            auto task = std::make_shared<std::packaged_task<ReturnT()>>(ff);

            // Package the task in a wrapper with a common void result
            // plus a skip flag for destruction without running the task
            std::packaged_task<void(bool)> resultTask([task](bool skip) {
                if (!skip)
                    (*task)();
                // task automatically deleted when shared_ptr goes out of scope
            });

            // Add task to queue and return future
            std::future<ReturnT> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_exit)
                {
                    // Pool is shutting down - reject new tasks
                    throw std::runtime_error("Cannot enqueue task: thread pool is shutting down");
                }
                m_tasks.emplace(std::move(resultTask));
            }
            m_cv.notify_one();
            return res;
        }

        void shutdown()
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                
                // Already shutting down or shut down
                if (m_exit)
                    return;
                
                m_exit = true;

                // Clear all remaining tasks (they will be destroyed automatically)
                while (!m_tasks.empty())
                {
                    m_tasks.pop();
                }
            }

            // Notify all threads to exit
            m_cv.notify_all();

            // Wait for threads to end
            for (std::thread& worker : m_vecThreads)
            {
                if (worker.joinable())
                    worker.join();
            }
        }

        ~CThreadPool() noexcept
        {
            try
            {
                shutdown();
            }
            catch (...)
            {
                // Must suppress exceptions to prevent std::terminate().
                // This should only happen if mutex operations fail (system error).
                dassert(false && "Exception during thread pool destruction");
            }
        }

        static CThreadPool& getDefaultThreadPool()
        {
            static CThreadPool DefaultThreadPool(Clamp<int>(2, std::thread::hardware_concurrency(), 16));
            return DefaultThreadPool;
        }

    private:
        std::vector<std::thread>                   m_vecThreads;
        std::queue<std::packaged_task<void(bool)>> m_tasks;
        std::mutex                                 m_mutex;
        std::condition_variable                    m_cv;
        bool                                       m_exit = false;
    };

    template <typename... Args>
    auto async(Args&&... args)
    {
        return CThreadPool::getDefaultThreadPool().enqueue(std::forward<Args>(args)...);
    }
}            // namespace SharedUtil
