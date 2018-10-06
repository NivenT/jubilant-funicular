#ifndef NTA_WINDOW_H_INCLUDED
#define NTA_WINDOW_H_INCLUDED

#include <atomic>

#include <SDL2/SDL.h>

#include "nta/MyEngine.h"
#include "nta/Wrapper.h"

namespace nta {
    class WindowManager;
    typedef utils::Wrapper<uint32_t, struct WIDTAG> WindowID;
    /// Flags used for creating a window
    enum WindowFlags {
        INVISIBLE = 0x1, 
        FULLSCREEN = 0x2, 
        BORDERLESS = 0x4, 
        NOTRESIZEABLE = 0x8,
        HIGHDPI = 0x10
    };
    /// Key for unlocking the GetSDLWindow() "private" function of class Window
    class GetSDLWindowKey {
        friend class ScreenManager;
        GetSDLWindowKey() {}
        GetSDLWindowKey(const GetSDLWindowKey&);
        GetSDLWindowKey& operator=(const GetSDLWindowKey&);
    };
    /// Represent a window
    class Window {
    private:
        static std::atomic<WindowID> m_keyboard_focus;
        static std::atomic<WindowID> m_mouse_focus;
        /// creates a window
        void createWindow(crstring title, int width, int height, int flags = 0);
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
        SDL_Window* getSDLWindow(GetSDLWindowKey key) const;
        /// returns the window's dimensions
        glm::vec2 getDimensions() const;
        /// returns the window's title
        std::string getTitle() const;
        /// returns the width of the window
        int getWidth() const;
        /// returns the height of the window
        int getHeight() const;
        bool hasKeyboardFocus() const { return m_keyboard_focus.load() == SDL_GetWindowID(m_window); }
        bool hasMouseFocus() const { return m_mouse_focus.load() == SDL_GetWindowID(m_window); }
        /// resizes the window
        void resize(int width, int height);
        /// updates the window's stored dimensions
        void setDimensions(int width, int height);
        /// updates the screen
        void swapBuffers() const;
        
        static void setKeyboardFocus(const Window* win) { m_keyboard_focus = SDL_GetWindowID(win->m_window); }
        static void setMouseFocus(const Window* win) { m_mouse_focus = SDL_GetWindowID(win->m_window); }

        friend class WindowManager;
    };
}

#endif // NTA_WINDOW_H_INCLUDED
