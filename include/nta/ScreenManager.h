#ifndef NTA_SCREENMANAGER_H_INCLUDED
#define NTA_SCREENMANAGER_H_INCLUDED

#include <vector>
#include <map>
#include <mutex>

#include "nta/Screen.h"
#include "nta/FPSLimiter.h"
#include "nta/Window.h"
#include "nta/GLSLProgram.h"
#include "nta/InputManager.h"
#include "nta/ResourceManager.h"

namespace nta {
    /// Manages a collection of screens
    /// \todo (?) Move m_glslMap and m_input to Window?
    class ScreenManager {
    private:
        /// Updates the state of m_input
        void update_input();

        static std::mutex m_window_creation_lock;
        /// Lock for access the event queue
        static std::mutex m_event_lock;

        /// Collection of GLSLProgram
        std::map<std::string, GLSLProgram> m_glslMap;
        /// the screens
        std::vector<Screen*> m_screens;
        /// Keeps track of all input received in this window
        InputManager m_input;
        /// used to cap the FPS
        FPSLimiter m_limiter;
        /// the main window used by the manager
        Window* m_window;
        /// the index of the currently active screen
        int m_currScreen = -1;
    public:
        /// sets the max fps and the window to use
        ScreenManager(crstring title, float maxFPS, int width = 640, int height = 480);
        /// basic destructor
        ~ScreenManager();
        /// Gets GLSLProgram at specified path
        ///
        /// Assumes vertex shader is (progPath + ".vert") and
        /// fragment shader is (progPath + ".frag")
        GLSLProgram* getGLSLProgram(crstring progPath);
        /// Gets GLSLProgram, specifying explicitly where the vertex and fragment
        /// shaders are. 
        ///
        /// Uses name as the key in the map
        GLSLProgram* getGLSLProgram(crstring name, crstring vert, crstring frag);
        /// returns the active screen
        Screen* getCurrScreen() const;
        /// returns the window
        const Window* getWindow() const { return m_window; }
        /// returns the InputManager
        const InputManager&  getInput() const { return m_input; }
        /// returns the current fps
        float getFPS() const;
        /// returns true if this manager is responsible for handling the event
        bool owns_event(const SDL_Event& event) const;
        /// adds a screen and sets some of its properties
        void addScreen(Screen* newScreen, int escIndex = -1, int xIndex = -1, crstring title = "");
        /// switches the to a new screen
        void switchScreen(int newIndex);
        /// destroys screens
        void destroy();
        /// runs screen logic (render, update, handleInput, etc.)
        ///
        /// initFocusData is the input to onFocus for the first screen
        void run(void* initFocusData = nullptr);
    };
}

#endif // NTA_SCREENMANAGER_H_INCLUDED
