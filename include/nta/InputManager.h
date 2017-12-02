#ifndef INPUTMANAGER_H_INCLUDED
#define INPUTMANAGER_H_INCLUDED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <unordered_map>

namespace nta {
    ///represents the way a mouse wheel was rolled
    enum MouseWheelMotion{STATIONARY = 0, SCROLL_UP = 1, SCROLL_DOWN = -1};
    ///keeps track of all input
    class InputManager {
    private:
        ///stores whether each key is pressed or not
        static std::unordered_map<unsigned int, bool>   m_keyMap;
        ///stores whether the key was pressed last frame or not
        static std::unordered_map<unsigned int, bool>   m_prevKeyMap;
        ///stores the location of the mouse in mouse coordinates
        static glm::vec2                                m_mouseCoords;
        ///stores the motion of the mouse wheel
        static MouseWheelMotion                         m_mouseWheelMotion;
    public:
        ///returns the mouse's coordinates
        static glm::vec2                                getMouseCoords();
        ///returns the mouse's coordinates with the y axis flipped (0 represents the bottom of the screen instead of top)
        static glm::vec2                                getMouseCoordsStandard(int height);
        ///returns the mouse wheel's motion
        static MouseWheelMotion                         getMouseWheelMotion();
        ///returns whether or not specified key is pressed
        static bool                                     isPressed(unsigned int key);
        ///returns whether or not the key was just pressed this frame
        static bool                                     justPressed(unsigned int key);
        ///returns whether or not the key was just released this frame
        static bool                                     justReleased(unsigned int key);
        ///tells InputManager that specified key was pressed
        static void                                     pressKey(unsigned int key);
        ///tells InputManager that specified key was released
        static void                                     releaseKey(unsigned int key);
        ///tells InputManager where the mouse is
        static void                                     setMouseCoords(float x, float y);
        ///tells InputManager how the wheel is rolling
        static void                                     setMouseWheelMotion(const MouseWheelMotion& motion);
        ///updates the state of m_KeyMap
        static void                                     update(SDL_Event& event);
        ///updates the state of m_prevKeyMap
        static void                                     updatePrev();
    };
}

#endif // INPUTMANAGER_H_INCLUDED
