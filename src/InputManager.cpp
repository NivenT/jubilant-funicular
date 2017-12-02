#include "InputManager.h"

namespace nta {
    std::unordered_map<unsigned int, bool> InputManager::m_keyMap;
    std::unordered_map<unsigned int, bool> InputManager::m_prevKeyMap;
    glm::vec2                              InputManager::m_mouseCoords;
    MouseWheelMotion                       InputManager::m_mouseWheelMotion;
    glm::vec2 InputManager::getMouseCoords() {
        return m_mouseCoords;
    }
    glm::vec2 InputManager::getMouseCoordsStandard(int height) {
        return glm::vec2(m_mouseCoords.x,height-m_mouseCoords.y);
    }
    MouseWheelMotion InputManager::getMouseWheelMotion() {
        return m_mouseWheelMotion;
    }
    bool InputManager::isPressed(unsigned int key) {
        auto it = m_keyMap.find(key);
        if (it != m_keyMap.end()) {
            return m_keyMap[key];
        } else {
            return false;
        }
    }
    bool InputManager::justPressed(unsigned int key) {
        auto it = m_keyMap.find(key);
        if (it != m_keyMap.end()) {
            return (m_keyMap[key] && !m_prevKeyMap[key]);
        } else {
            return false;
        }
    }
    bool InputManager::justReleased(unsigned int key) {
        auto it = m_keyMap.find(key);
        if (it != m_keyMap.end()) {
            return (m_prevKeyMap[key] && !m_keyMap[key]);
        } else {
            return false;
        }
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
