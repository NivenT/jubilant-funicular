#ifndef FPSLIMITER_H_INCLUDED
#define FPSLIMITER_H_INCLUDED

#include "Timer.h"

namespace nta {
    /// used to cap the fps of the program at a specific value
    class FPSLimiter : public Timer {
    private:
        /// most recently calculated fps
        float m_fps;
        /// maximum allowed fps
        float m_maxFPS;
    public:
        /// constructor and destructor
        FPSLimiter(float max = 60) : m_maxFPS(max) {}
        ~FPSLimiter();
        /// sets maximum allowed fps
        void setMaxFPS(float maxFPS);
        /// gets most recently calculated fps
        float getFPS() const;
        /// ends fps calculations, delaying if necessary to cap fps
        long double end();
    };
}

#endif // FPSLIMITER_H_INCLUDED
