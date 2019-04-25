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
                        std::pair<void*, std::packaged_task<void()>> task;
                        {
                            std::unique_lock<std::mutex> lock(m_mutex);
                            m_cv.wait(lock, [this] { return m_exit || !m_tasks.empty(); });
                            if (m_exit && m_tasks.empty())
                                return;
                            task = std::move(m_tasks.front());
                            m_tasks.pop();
                        }
                        task.second();
                        delete task.first;
                    }
                });
            }
        };

        template <typename Func, typename... Args>
        auto enqueue(Func&& f, Args&&... args)
        {
            // todo: use the commented line once we have c++17...
            // using ReturnT = std::invoke_result_t<Func, Args...>;
            using ReturnT = std::result_of_t<Func(Args...)>;
            auto                       ff = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
            auto*                      task = new std::packaged_task<ReturnT()>(ff);
            std::packaged_task<void()> resultTask([task] { (*task)(); });

            std::future<ReturnT> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                // Note: hacky casting required, as msvc does not allow conversion of
                // std::packaged_task<T()> to std::packaged_task<void()>
                m_tasks.emplace((void*)task, std::move(resultTask));
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
                auto pair = std::move(m_tasks.front());
                delete pair.first;
            } while (true);
        }

        static CThreadPool& getDefaultThreadPool()
        { 
            static CThreadPool DefaultThreadPool(Clamp<int>(2, std::thread::hardware_concurrency(), 16));
            return DefaultThreadPool;
        }

    private:
        std::vector<std::thread>                                 m_vecThreads;
        std::queue<std::pair<void*, std::packaged_task<void()>>> m_tasks;
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
