#pragma once

#include <deque>
#include "cppevent\cppevent.hpp"

namespace cppevent_demo
{
    template <typename T>
    class observable_queue
    {
        std::deque<T> m_deque;
    public:
        cppevent::event<observable_queue, void(std::size_t)> OnChanged;

        void put(const T& t)
        {
            m_deque.push_front(t);
            OnChanged(m_deque.size());
        }
        bool take(T& t)
        {
            if (m_deque.empty())
            {
                return false;
            }
            t = m_deque.front();
            m_deque.pop_front();
            OnChanged(m_deque.size());
            return true;
        }
    };
}
