#ifndef NTA_INPUTMANAGER_H_INCLUDED
#define NTA_INPUTMANAGER_H_INCLUDED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <unordered_map>

namespace nta {
    class ScreenManager;
    /// Key unlocking the InputManager constructor
    class CreateInputManagerKey {
        friend class ScreenManager;
        CreateInputManagerKey() {}
        CreateInputManagerKey(const CreateInputManagerKey&);
        CreateInputManagerKey& operator=(const CreateInputManagerKey&);
    };
    /// represents the way a mouse wheel was rolled
    enum MouseWheelMotion{STATIONARY = 0, SCROLL_UP = 1, SCROLL_DOWN = -1};
    /// keeps track of all input
    class InputManager {
    private:
        /// stores whether each key is pressed or not
        std::unordered_map<unsigned int, bool> m_keyMap;
        /// stores whether the key was pressed last frame or not
        std::unordered_map<unsigned int, bool> m_prevKeyMap;
        /// stores the location of the mouse in mouse coordinates
        glm::vec2 m_mouseCoords;
        /// stores the motion of the mouse wheel
        MouseWheelMotion m_mouseWheelMotion;
    public:
        InputManager(CreateInputManagerKey) {}
        /// returns the mouse's coordinates
        glm::vec2 getMouseCoords() const;
        /// returns the mouse's coordinates with the y axis flipped (0 represents the bottom of the screen instead of top)
        glm::vec2 getMouseCoordsStandard(int height) const;
        /// returns the mouse wheel's motion
        MouseWheelMotion getMouseWheelMotion() const;
        /// returns whether or not specified key is pressed
        bool isPressed(unsigned int key) const;
        /// returns whether or not the key was just pressed this frame
        bool justPressed(unsigned int key) const;
        /// returns whether or not the key was just released this frame
        bool justReleased(unsigned int key) const;
        /// tells InputManager that specified key was pressed
        void pressKey(unsigned int key);
        /// tells InputManager that specified key was released
        void releaseKey(unsigned int key);
        /// tells InputManager where the mouse is
        void setMouseCoords(float x, float y);
        /// tells InputManager how the wheel is rolling
        void setMouseWheelMotion(const MouseWheelMotion& motion);
        /// updates internal state
        void update(SDL_Event& event);
        /// updates key information
        void update_keys(SDL_Event& event);
        /// update mouse information
        void update_mouse(SDL_Event& event);
        /// updates the state of m_prevKeyMap
        void updatePrev();
    };
}

#endif // NTA_INPUTMANAGER_H_INCLUDED
