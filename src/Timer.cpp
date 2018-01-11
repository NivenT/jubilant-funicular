#include "nta/Timer.h"

namespace nta {
    Timer::Timer() {
    }
    Timer::~Timer() {
    }
    void Timer::begin() {
        m_startPoint = std::chrono::high_resolution_clock::now();
    }
    long double Timer::end() const {
        return std::chrono::duration<long double, std::nano>(std::chrono::high_resolution_clock::now() - m_startPoint).count();
    }
}
