#include "nta/SystemManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string, GLSLProgram*> SystemManager::m_glslMap;
    std::map<std::string, Window*>      SystemManager::m_windowMap;
    GLSLProgram* SystemManager::getGLSLProgram(crstring progPath) {
        //TODO: Support other file layouts
        std::string folder = "";
        if (progPath.find("shaders/") != 0) {
            folder += "shaders/";
        }
        if (m_glslMap.find(folder+progPath) == m_glslMap.end()) {
            m_glslMap[folder+progPath] = new GLSLProgram;
            m_glslMap[folder+progPath]->compileShaders(folder+progPath);
        }
        return m_glslMap[folder+progPath];
    }
    Window* SystemManager::getWindow(crstring windowTitle, int flags) {
        if (m_windowMap.find(windowTitle) == m_windowMap.end()) {
            m_windowMap[windowTitle] = new Window;
            m_windowMap[windowTitle]->createWindow(windowTitle, 640, 480, flags);
        }
        return m_windowMap[windowTitle];
    }
    void SystemManager::destroy() {
        Logger::writeToLog("Destroying SystemManager...");
        for (auto pair : m_glslMap) {
            Logger::writeToLog("Deleting GLSL program: " + pair.first + "...");
            delete pair.second;
            Logger::writeToLog("Deleted program");
        }
        for (auto pair : m_windowMap) {
            Logger::writeToLog("Deleting window: " + pair.first + "...");
            delete pair.second;
            Logger::writeToLog("Deleted window");
        }
        m_glslMap.clear();
        m_windowMap.clear();
        Logger::writeToLog("Destroyed SystemMangaer");
    }
}
