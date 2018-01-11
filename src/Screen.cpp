#include <GL/glew.h>

#include "nta/Screen.h"
#include "nta/InputManager.h"
#include "nta/SystemManager.h"
#include "nta/Logger.h"

namespace nta {
    Screen::Screen() {
    }
    Screen::~Screen() {
    }
    ScreenState Screen::getState() const {
        return m_state;
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
    void Screen::setManager(ScreenManager* manager, SetManagerKey key) {
        m_manager = manager;
    }
    void Screen::setIndices(int index, int escIndex, int xIndex) {
        m_index = index;
        m_escIndex = escIndex;
        m_xIndex = xIndex;
    }
    void Screen::setWindow(crstring title) {
        m_window = SystemManager::getWindow(title);
    }
    void Screen::handleInput() {
        static SDL_Event event;

        InputManager::updatePrev();
        InputManager::setMouseWheelMotion(MouseWheelMotion::STATIONARY);
        while (SDL_PollEvent(&event)) {
            InputManager::update(event);
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
        } else if (InputManager::justPressed(SDLK_BACKQUOTE)) {
            m_window->screenshot();
        }
    }
    void Screen::onFocus() {
        m_state = ScreenState::RUNNING;
    }
    void Screen::offFocus() {
    }
}
