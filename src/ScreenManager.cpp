#include <queue>

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
    std::mutex ScreenManager::m_event_lock;
    ScreenManager::ScreenManager(crstring title, float maxFPS, int width, int height) : m_input(CreateInputManagerKey()) {
        std::lock_guard<std::mutex> g(m_window_creation_lock);
        m_window = WindowManager::getWindow(title, width, height);
        m_limiter.setMaxFPS(maxFPS);
    }
    ScreenManager::~ScreenManager() {
        if (!m_screens.empty()) destroy();
    }
    Screen* ScreenManager::getCurrScreen() const {
        /// \todo write log error if m_currScreen is out of range
        return utils::in_range<int>(m_currScreen, 0, m_screens.size()-1) ? m_screens[m_currScreen] : nullptr;
    }
    float ScreenManager::getFPS() const {
        return m_limiter.getFPS();
    }
    void ScreenManager::setResourceFolders(const utils::Path& tex_fldr,
                                           const utils::Path& glsl_fldr,
                                           const utils::Path& font_fldr) {
        m_context_data.setTextureFolder(tex_fldr);
        m_context_data.setGLSLFolder(glsl_fldr);
        m_context_data.setFontFolder(font_fldr);
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
            getCurrScreen()->offFocus();
            m_currScreen = -1;
        }
    }
    bool ScreenManager::owns_event(const SDL_Event& event) const {
        switch(event.type) {
        case SDL_WINDOWEVENT: return m_window->getID() == event.window.windowID;
        case SDL_KEYDOWN: case SDL_KEYUP: return m_window->hasKeyboardFocus();
        case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: case SDL_MOUSEWHEEL: return m_window->hasMouseFocus();
        }
        return true;
    }
    void ScreenManager::update_input() {
        //SDL_PumpEvents();
        std::lock_guard<std::mutex> g(m_event_lock);

        if (m_window->hasKeyboardFocus()) {
            m_input.updatePrev();
        }
        if (m_window->hasMouseFocus()) {
            m_input.setMouseWheelMotion(MouseWheelMotion::STATIONARY);
        }

        std::queue<SDL_Event> skipped_events;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (!owns_event(e)) {
                skipped_events.push(e);
                continue;
            }

            switch(e.type) {
            case SDL_WINDOWEVENT: {
                switch(e.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    glViewport(0, 0, e.window.data1, e.window.data2);
                    m_window->setDimensions(e.window.data1, e.window.data2);
                    break;
                case SDL_WINDOWEVENT_ENTER:
                    Window::setMouseFocus(e.window.windowID);
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    Window::setKeyboardFocus(e.window.windowID);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    getCurrScreen()->close();
                    break;
                }
            } break;
            }

            m_input.update(e);
            #ifdef NTA_USE_IMGUI
                ImGui_ImplSdlGL3_ProcessEvent(&e);
            #endif
        }
        while (!skipped_events.empty()) {
            SDL_PushEvent(&skipped_events.front());
            skipped_events.pop();
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
        Logger::indent();
        for (auto screen : m_screens) {
            delete screen;
        }
        m_screens.clear();
        m_context_data.destroy();
        WindowManager::destroyWindow(m_window->getTitle());
        Logger::unindent();
        Logger::writeToLog("Destroyed ScreenManager");
    }
}
