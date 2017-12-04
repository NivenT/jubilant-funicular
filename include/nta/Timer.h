#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono>

namespace nta {
    ///represents a timer
    class Timer {
    protected:
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> m_startPoint;
    public:
        ///constructor and destructor
        Timer();
        virtual ~Timer();
        ///begins timer
        virtual void begin();
        ///return time since beginning of timer in nanoseconds
        virtual long double end() const;
    };
}

#endif // TIMER_H_INCLUDED
