#ifndef NTA_WINDOWMANAGER_H_INCLUDED
#define NTA_WINDOWMANAGER_H_INCLUDED

#include <map>

#include "nta/Window.h"

namespace nta {
    /// Manages Windows (or GL contexts if you prefer)
    class WindowManager {
    private:
        /// Collection of Window
        static std::map<std::string, Window> m_windowMap;
    public:
        /// Gets a Window with specified title, width, height, and flags
        static Window* getWindow(crstring windowTitle, int width = 640, 
                                 int height = 480, int flags = 0);
        static Window* getWindow(crstring windowTitle, int flags) {
            return getWindow(windowTitle, 640, 480, flags);
        }
        static void destroyWindow(crstring title);
        /// Destorys the manager, freeing all necessary data
        static void destroy();
    };
}

#endif // NTA_WINDOWMANAGER_H_INCLUDED
