#ifndef SCREENMANAGER_H_INCLUDED
#define SCREENMANAGER_H_INCLUDED

#include <vector>

#include "Screen.h"
#include "FPSLimiter.h"
#include "Window.h"

namespace nta {
    ///Manages a collection of screens
    class ScreenManager {
    private:
        ///the screens
        std::vector<Screen*> m_screens;
        ///used to cap the FPS
        FPSLimiter m_limiter;
        ///the main window used by the manager
        Window* m_window;
        ///the index of the currently active screen
        int m_currScreen = -1;
    public:
        ///sets the max fps and the window to use
        ScreenManager(crstring title, float maxFPS);
        ///basic destructor
        ~ScreenManager();
        ///returns the active screen
        Screen* getCurrScreen() const;
        ///adds a screen and sets some of its properties
        void addScreen(Screen* newScreen, int escIndex = -1, int xIndex = -1, crstring title = "");
        ///switches the to a new screen
        void switchScreen(int newIndex);
        ///destroys screens
        void destroy();
        ///runs screen logic (render, update, handleInput, etc.)
        void run();
    };
}

#endif // SCREENMANAGER_H_INCLUDED
