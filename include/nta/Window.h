#ifndef NTA_WINDOW_H_INCLUDED
#define NTA_WINDOW_H_INCLUDED

#include <SDL2/SDL.h>

#include "MyEngine.h"

namespace nta {
    class SystemManager;
    /// Flags used for creating a window
    enum WindowFlags {INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4, NOTRESIZEABLE = 0x8,
                      HIGHDPI = 0x10};
    /// Represent a window
    class Window {
    private:
        /// creates a window
        void createWindow(crstring title, int width, int height, int flags = HIGHDPI);
        /// the window
        SDL_Window* m_window;
        /// the title of the window
        std::string m_title;
        /// the dimensions of the window
        int m_width, m_height;
    public:
        /// constructor and destructor
        Window();
        ~Window();
        /// returns the underlying window
        // Not sure how I feel about this
        SDL_Window* getSDLWindow() const;
        /// returns the window's dimensions
        glm::vec2 getDimensions() const;
        /// returns the window's title
        std::string getTitle() const;
        /// returns the width of the window
        int getWidth() const;
        /// returns the height of the window
        int getHeight() const;
        /// updates the window's stored dimensions
        void setDimensions(int width, int height);
        /// updates the screen
        void swapBuffers() const;
        /// stores a screenshot
        void screenshot() const;
        friend class SystemManager;
    };
}

#endif // NTA_WINDOW_H_INCLUDED
