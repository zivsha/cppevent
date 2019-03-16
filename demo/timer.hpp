#pragma once

#include "cppevent/cppevent.hpp"
#include <thread>
#include <future>
#include <chrono>

namespace cppevent_demo
{
    class Timer
    {
        std::chrono::high_resolution_clock::time_point m_start;
        std::chrono::milliseconds m_duration;
        std::future<void> m_thread;
    public:
        Timer(std::chrono::milliseconds duration) :
            m_start(std::chrono::high_resolution_clock::now()),
            m_duration(duration)
        {
        }
        ~Timer()
        {
            if (m_thread.valid())
            {
                m_thread.wait_for(m_duration * 2);
            }
        }
        cppevent::event<Timer, void()> Elapsed;

        void Start()
        {
            m_thread = std::async(std::launch::async, [this]() {
                while (std::chrono::high_resolution_clock::now() - m_start < m_duration);
                Elapsed();
            });
        }
    };
}