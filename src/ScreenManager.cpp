#include <GL/glew.h>

#ifdef NTA_USE_IMGUI
    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_sdl_gl3.h>
#endif

#include "nta/ScreenManager.h"
#include "nta/WindowManager.h"
#include "nta/CallbackManager.h"
#include "nta/Logger.h"
#include "nta/utils.h"

namespace nta {
    std::mutex ScreenManager::m_window_creation_lock;
    ScreenManager::ScreenManager(crstring title, float maxFPS, int width, int height) : m_input(CreateInputManagerKey()) {
        std::lock_guard<std::mutex> g(m_window_creation_lock);
        m_window = WindowManager::getWindow(title, width, height);
        m_limiter.setMaxFPS(maxFPS);
    }
    ScreenManager::~ScreenManager() {
        if (!m_screens.empty()) destroy();
    }
    GLSLProgram* ScreenManager::getGLSLProgram(crstring progPath) {
        if (m_glslMap.find(progPath) == m_glslMap.end()) {
            m_glslMap[progPath].compileShaders(progPath);
        }
        return &m_glslMap[progPath];
    }
    GLSLProgram* ScreenManager::getGLSLProgram(crstring name, crstring vert, crstring frag) {
        if (m_glslMap.find(name) == m_glslMap.end()) {
            m_glslMap[name].compileShaders(vert, frag);
        }
        return &m_glslMap[name];
    }
    Screen* ScreenManager::getCurrScreen() const {
        /// \todo write log error if m_currScreen is out of range
        return utils::in_range<int>(m_currScreen, 0, m_screens.size()-1) ? m_screens[m_currScreen] : nullptr;
    }
    float ScreenManager::getFPS() const {
        return m_limiter.getFPS();
    }
    void ScreenManager::addScreen(Screen* newScreen, int escIndex, int xIndex, crstring title) {
        Logger::writeToLog("Adding screen " + utils::to_string(m_screens.size()) + 
                           " (\"" + newScreen->getName() + "\") to ScreenManager...");
        Logger::indent();
        m_currScreen = m_screens.empty() ? 0 : m_currScreen;
        newScreen->setManager(this, SetManagerKey());
        newScreen->setIndices(m_screens.size(), escIndex, xIndex, SetIndicesKey());
        newScreen->setWindow((title == "") ? m_window->getTitle() : title, SetWindowKey());
        newScreen->init();
        m_screens.push_back(newScreen);
        check_error();
        Logger::unindent();
        Logger::writeToLog("Added screen");
    }
    void ScreenManager::switchScreen(int newIndex) {
        if (utils::in_range<int>(newIndex, 0, m_screens.size()-1)) {
            Logger::writeToLog("Switching from screen " + utils::to_string(m_currScreen) +
                               " (\"" + getCurrScreen()->getName() + "\") to screen " + 
                               utils::to_string(newIndex) + " (\"" + m_screens[newIndex]->getName() +
                               "\")...");
            Logger::indent();
            getCurrScreen()->offFocus();
            Logger::writeToLog("called offFocus");
            check_error();
            ScreenSwitchInfo info(getCurrScreen()->getSwitchData(), m_currScreen);
            m_currScreen = newIndex;
            getCurrScreen()->onFocus(info);
            Logger::writeToLog("called onFocus");
            check_error();
            Logger::unindent();
            Logger::writeToLog("Switched screen");
        } else if (newIndex == -1) {
            Logger::writeToLog("Exiting ScreenManager...");
            getCurrScreen()->offFocus(); // necessary?
            m_currScreen = -1;
        }
    }
    void ScreenManager::update_input() {
        SDL_Event event;

        m_input.updatePrev();
        m_input.setMouseWheelMotion(MouseWheelMotion::STATIONARY);
        while (SDL_PollEvent(&event)) {
            m_input.update(event);
            #ifdef NTA_USE_IMGUI
                ImGui_ImplSdlGL3_ProcessEvent(&event);
            #endif
            switch(event.type) {
            case SDL_QUIT:
                getCurrScreen()->quit();
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    m_window->setDimensions(event.window.data1, event.window.data2);
                }
                break;
            }
        }
        if (m_input.justPressed(SDLK_ESCAPE)) {
            getCurrScreen()->esc();
        }
    }
    void ScreenManager::run(void* initFocusData) {
        Screen* currScreen = nullptr;
        if (m_currScreen != -1) {
            getCurrScreen()->onFocus(ScreenSwitchInfo(initFocusData));
        }
        while (m_currScreen != -1) {
            currScreen = getCurrScreen();
            // This while loop used to be neat
            while (currScreen->getState() == ScreenState::RUNNING) {
                m_limiter.begin();
                CallbackManager::increment_frame();
                update_input();
                currScreen->update();
                #ifdef NTA_USE_IMGUI
                    ImGui_ImplSdlGL3_NewFrame(m_window->getSDLWindow(GetSDLWindowKey()));
                #endif
                currScreen->render();
                ErrorManager::handle_errors();
                m_limiter.end();
            }
            switch(currScreen->getState()) {
            case ScreenState::SWITCH:       switchScreen(currScreen->getNextIndex()); break;
            case ScreenState::SWITCH_ESC:   switchScreen(currScreen->getEscIndex()); break;
            case ScreenState::SWITCH_X:     switchScreen(currScreen->getXIndex()); break;
            case ScreenState::NONE:         Logger::writeErrorToLog("state of screen " +
                                                                    utils::to_string(m_currScreen) +
                                                                    " is NONE",
                                                                    IMPOSSIBLE_BEHAVIOR); break;
            default: break; // should never happen
            }
        }
        Logger::writeToLog("Exited ScreenManager");
    }
    void ScreenManager::destroy() {
        Logger::writeToLog("Destroying ScreenManager...");
        for (auto screen : m_screens) {
            delete screen;
        }
        m_screens.clear();
        m_glslMap.clear();
        Logger::writeToLog("Destroyed ScreenManager...");
    }
}
