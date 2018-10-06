#include "nta/InputManager.h"

namespace nta {
    glm::vec2 InputManager::getMouseCoords() const {
        return m_mouseCoords;
    }
    glm::vec2 InputManager::getMouseCoordsStandard(int height) const {
        return glm::vec2(m_mouseCoords.x,height-m_mouseCoords.y);
    }
    MouseWheelMotion InputManager::getMouseWheelMotion() const {
        return m_mouseWheelMotion;
    }
    bool InputManager::isPressed(unsigned int key) const {
        auto it = m_keyMap.find(key);
        return it == m_keyMap.end() ? false : it->second;
    }
    bool InputManager::justPressed(unsigned int key) const {
        auto it = m_prevKeyMap.find(key);
        return it == m_prevKeyMap.end() ? isPressed(key) : isPressed(key) && !it->second;
    }
    bool InputManager::justReleased(unsigned int key) const {
        auto it = m_prevKeyMap.find(key);
        return it == m_prevKeyMap.end() ? false : !isPressed(key) && it->second;
    }
    void InputManager::pressKey(unsigned int key) {
        m_keyMap[key] = true;
    }
    void InputManager::releaseKey(unsigned int key) {
        m_keyMap[key] = false;
    }
    void InputManager::setMouseCoords(float x, float y) {
        m_mouseCoords.x = x;
        m_mouseCoords.y = y;
    }
    void InputManager::setMouseWheelMotion(const MouseWheelMotion& motion) {
        m_mouseWheelMotion = motion;
    }
    void InputManager::update(SDL_Event& event) {
        switch(event.type) {
        case SDL_KEYDOWN: pressKey(event.key.keysym.sym); break;
        case SDL_KEYUP: releaseKey(event.key.keysym.sym); break;
        case SDL_MOUSEMOTION: setMouseCoords(event.motion.x, event.motion.y); break;
        case SDL_MOUSEBUTTONDOWN: pressKey(event.button.button); break;
        case SDL_MOUSEBUTTONUP: releaseKey(event.button.button); break;
        case SDL_MOUSEWHEEL: setMouseWheelMotion((nta::MouseWheelMotion)glm::sign(event.wheel.y)); break;
        }
    }
    void InputManager::updatePrev() {
        m_prevKeyMap.clear();
        m_prevKeyMap.insert(m_keyMap.begin(), m_keyMap.end());
    }
};
