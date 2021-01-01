#include <stack>
#include <mutex>

#pragma once

namespace SharedUtil
{
    template <typename T>
    class ConcurrentStack
    {
    public:
        void push(const T& item)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_stack.push(item);
        }
        void pop()
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_stack.pop();
        }
        T top() const
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            return m_stack.top();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            return m_stack.empty();
        }

    private:
        mutable std::mutex m_mutex;
        std::stack<T>      m_stack;
    };
}            // namespace SharedUtil
