#include "nta/SystemManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string, GLSLProgram*> SystemManager::m_glslMap;
    std::map<std::string, Window*>      SystemManager::m_windowMap;
    GLSLProgram* SystemManager::getGLSLProgram(crstring progPath) {
        if (m_glslMap.find(progPath) == m_glslMap.end()) {
            m_glslMap[progPath] = new GLSLProgram;
            m_glslMap[progPath]->compileShaders(progPath);
        }
        return m_glslMap[progPath];
    }
    GLSLProgram* SystemManager::getGLSLProgram(crstring name, crstring vert, crstring frag) {
        if (m_glslMap.find(name) == m_glslMap.end()) {
            m_glslMap[name] = new GLSLProgram;
            m_glslMap[name]->compileShaders(vert, frag);
        }
        return m_glslMap[name];
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
        Logger::indent();
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
        Logger::unindent();
        Logger::writeToLog("Destroyed SystemMangaer");
    }
}
