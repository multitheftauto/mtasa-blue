/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.ThreadPool.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <thread>
#include <queue>
#include <future>
#include <vector>
#include <algorithm>

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
                        // Run the task
                        task(false);
                    }
                });
            }
        };

        template <typename Func, typename... Args>
        auto enqueue(Func&& f, Args&&... args)
        {
            using ReturnT = std::invoke_result_t<Func, Args...>;
            auto  ff = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
            auto* task = new std::packaged_task<ReturnT()>(ff);

            // Package the task in a wrapper with a common void result
            // plus a skip flag for destruction without running the task
            std::packaged_task<void(bool)> resultTask([task](bool skip) {
                if (!skip)
                    (*task)();
                delete task;
            });

            // Add task to queue and return future
            std::future<ReturnT> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_tasks.emplace(std::move(resultTask));
            }
            m_cv.notify_one();
            return res;
        }

        ~CThreadPool()
        {
            // Notify all threads to exit
            m_exit = true;
            m_cv.notify_all();
            // Wait for threads to end
            for (std::thread& worker : m_vecThreads)
            {
                worker.join();
            }
            // Cleanup
            do
            {
                if (m_tasks.empty())
                    break;
                // Run each task but skip execution of the actual
                // function (-> just delete the task)
                auto task = std::move(m_tasks.front());
                task(true);
            } while (true);
        }

        static CThreadPool& getDefaultThreadPool()
        {
            static CThreadPool DefaultThreadPool(Clamp<int>(2, std::thread::hardware_concurrency(), 16));
            return DefaultThreadPool;
        }

    private:
        std::vector<std::thread>                                 m_vecThreads;
        std::queue<std::packaged_task<void(bool)>>               m_tasks;
        std::mutex                                               m_mutex;
        std::condition_variable                                  m_cv;
        bool                                                     m_exit = false;
    } ;

    template <typename... Args>
    auto async(Args&&... args)
    {
        return CThreadPool::getDefaultThreadPool().enqueue(std::forward<Args>(args)...);
    }
}            // namespace SharedUtil
