//
// Created by Minkyeu Kim on 7/22/23.
//

#ifndef SCOOP_TIMER_H
#define SCOOP_TIMER_H

#include <iostream>
#include <string>
#include <chrono>

//https://github.com/StudioCherno/Walnut/blob/master/Walnut/src/Walnut/Timer.h
namespace Lunar {
    class Timer
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;

    public:
        Timer()
        {
            Reset();
        }

        void Reset()
        {
            m_Start = std::chrono::high_resolution_clock::now();
        }

		// 현재 까지 경과한 시간을 반환
        float Elapsed()
        {
            const auto timePast = std::chrono::high_resolution_clock::now() - m_Start;
            const auto nanoSec = std::chrono::duration_cast<std::chrono::nanoseconds>(timePast) ;
            return nanoSec.count() * 0.001f * 0.001f * 0.001f;
        }

        float ElapsedMillis()
        {
            return Elapsed() * 1000.0f;
        }

    };

    class ScopedTimer
    {
    private:
        std::string m_Name;
        Timer m_Timer;

    public:
        ScopedTimer(const std::string& name)
            : m_Name(name)
        {}

        ~ScopedTimer()
        {
            float time = m_Timer.ElapsedMillis();
            std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
        }
    };
}

#endif //SCOOP_TIMER_H
