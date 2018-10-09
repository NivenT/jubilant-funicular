#include <SDL2/SDL.h>
#include <cmath>

#include "nta/FPSLimiter.h"

namespace nta {
    FPSLimiter::~FPSLimiter() {
        m_maxFPS = m_fps = 0;
    }
    void FPSLimiter::setMaxFPS(float maxFPS) {
        m_maxFPS = maxFPS;
    }
    float FPSLimiter::getFPS() const {
        return m_fps;
    }
    long double FPSLimiter::end() {
        long double frametime = Timer::end()/pow(10.,6); //elapsed time in milliseconds
        if (frametime < 1000./m_maxFPS) {
            SDL_Delay((1000./m_maxFPS) - frametime);
        }
        m_fps = 1./(Timer::end()/pow(10.,9));
        return Timer::end();
    }
}
