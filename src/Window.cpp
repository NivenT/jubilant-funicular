#include <GL/glew.h>
#include <fstream>

#ifdef NTA_USE_IMGUI
    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_sdl_gl3.h>
#endif

#include "nta/Window.h"
#include "nta/Logger.h"
#include "nta/IOManager.h"
#include "nta/utils.h"

namespace nta {
    Window::Window() {
    }
    Window::~Window() {
        m_width = m_height = 0;
        m_title = "";
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Window* Window::getSDLWindow(GetSDLWindowKey key) const {
        return m_window;
    }
    glm::vec2 Window::getDimensions() const {
        return glm::vec2(m_width, m_height);
    }
    std::string Window::getTitle() const {
        return m_title;
    }
    int Window::getWidth() const {
        return m_width;
    }
    int Window::getHeight() const {
        return m_height;
    }
    void Window::resize(int width, int height) {
        SDL_SetWindowSize(m_window, width, height);
        setDimensions(width, height);
    }
    void Window::setDimensions(int width, int height) {
        m_width = width;
        m_height = height;
    }
    void Window::createWindow(crstring title, int width, int height, int flags) {
        Logger::writeToLog("Creating window with dimensions " + utils::to_string(width) +
                           " x " + utils::to_string(height) + ": " + title + "...");
        m_width = width;
        m_height = height;
        m_title = title;
        int sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        if (flags & BORDERLESS) {
            sdlFlags |= SDL_WINDOW_BORDERLESS;
        }
        if (flags & FULLSCREEN) {
            sdlFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if (flags & INVISIBLE) {
            sdlFlags |= SDL_WINDOW_HIDDEN;
        }
        if (flags & NOTRESIZEABLE) {
            sdlFlags &= ~SDL_WINDOW_RESIZABLE;
        }
        if (flags & HIGHDPI) {
            sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
        }
        m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                    m_width, m_height, sdlFlags);
        check_error();
        if (!SDL_GL_CreateContext(m_window)) {
            Logger::writeErrorToLog("Failed to create an OpenGL context:\n\t"
                                    + utils::to_string(SDL_GetError()),
                                    GL_FAILURE);
        }
        glewExperimental = GL_TRUE; // I don't remember what this does or why it is needed
        
        const GLenum err = glewInit();
        if (err != GLEW_OK) {
            Logger::writeErrorToLog("Failed to initialize glew:\n\t" 
                                    + utils::to_string(glewGetErrorString(err)),
                                    GL_FAILURE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //SDL_GL_SetSwapInterval(1);
        check_error();
        #ifdef NTA_USE_IMGUI
            ImGui_ImplSdlGL3_Init(m_window);
        #endif
        Logger::writeToLog("Created window using OpenGL version "
                           + utils::to_string(glGetString(GL_VERSION)));
    }
    void Window::swapBuffers() const {
        #ifdef NTA_USE_IMGUI
            ImGui::Render();
            ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
        #endif
        SDL_GL_SwapWindow(m_window);
    }
}
