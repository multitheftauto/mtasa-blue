#include <list>
#include <mutex>

#pragma once

namespace SharedUtil
{
    template <typename T>
    class ConcurrentList
    {
    public:
        void push(const T& item)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_list.push_back(item);
        }

        T pop()
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            T firstElement = m_list.front();
            m_list.pop_front();
            return firstElement;
        }

        void remove(T element)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_list.remove(element);
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            return m_list.empty();
        }

        void lock() { m_mutex.lock(); }
        void unlock() { m_mutex.unlock(); }
        
        auto IterBegin() { return m_list.begin(); }
        auto IterEnd() { return m_list.end(); }


    private:
        mutable std::mutex m_mutex;
        std::list<T>       m_list;
    };
}            // namespace SharedUtil
