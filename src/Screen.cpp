#include <GL/glew.h>

#ifdef NTA_USE_IMGUI
    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_sdl_gl3.h>
#endif

#include "nta/Screen.h"
#include "nta/InputManager.h"
#include "nta/SystemManager.h"
#include "nta/Logger.h"

namespace nta {
    Screen::Screen(crstring name) : m_name(name) {
    }
    Screen::~Screen() {
    }
    ScreenState Screen::getState() const {
        return m_state;
    }
    std::string Screen::getName() const {
        return m_name;
    }
    int Screen::getEscIndex() const {
        return m_escIndex;
    }
    int Screen::getXIndex() const {
        return m_xIndex;
    }
    int Screen::getIndex() const {
        return m_index;
    }
    int Screen::getNextIndex() const {
        return m_nextIndex;
    }
    void* Screen::getSwitchData() const {
        return m_switchData;
    }
    void Screen::setManager(ScreenManager* manager, SetManagerKey key) {
        m_manager = manager;
    }
    void Screen::setIndices(int index, int escIndex, int xIndex, SetIndicesKey key) {
        m_index = index;
        m_escIndex = escIndex;
        m_xIndex = xIndex;
    }
    void Screen::setWindow(crstring title, SetWindowKey key) {
        m_window = SystemManager::getWindow(title);
    }
    void Screen::handleInput() {
        SDL_Event event;

        InputManager::updatePrev();
        InputManager::setMouseWheelMotion(MouseWheelMotion::STATIONARY);
        while (SDL_PollEvent(&event)) {
            InputManager::update(event);
            #ifdef NTA_USE_IMGUI
                ImGui_ImplSdlGL3_ProcessEvent(&event);
            #endif
            switch(event.type) {
            case SDL_QUIT:
                m_state = ScreenState::SWITCH_X;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    m_window->setDimensions(event.window.data1, event.window.data2);
                }
                break;
            }
        }
        if (InputManager::justPressed(SDLK_ESCAPE)) {
            m_state = ScreenState::SWITCH_ESC;
        }
    }
    void Screen::onFocus(const ScreenSwitchInfo& _) {
        m_state = ScreenState::RUNNING;
    }
    void Screen::offFocus() {
    }
}
