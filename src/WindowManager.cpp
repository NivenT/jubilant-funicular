#include "nta/WindowManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string, Window>      WindowManager::m_windowMap;
    Window* WindowManager::getWindow(crstring windowTitle, int width, int height, 
                                     int flags) {
        if (m_windowMap.find(windowTitle) == m_windowMap.end()) {
            m_windowMap[windowTitle].createWindow(windowTitle, width, height, flags);
        }
        return &m_windowMap[windowTitle];
    }
    void WindowManager::destroy() {
        Logger::writeToLog("Destroying WindowManager...");
        m_windowMap.clear();
        Logger::writeToLog("Destroyed WindowManager");
    }
}
