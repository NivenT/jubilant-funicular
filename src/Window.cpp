#include <GL/glew.h>
#include <fstream>

#include "nta/Window.h"
#include "nta/Logger.h"
#include "nta/IOManager.h"

namespace nta {
    Window::Window() {
    }
    Window::~Window() {
        m_width = m_height = 0;
        m_title = "";
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
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
    void Window::setDimensions(int width, int height) {
        m_width = width;
        m_height = height;
    }
    void Window::createWindow(crstring title, int width, int height, int flags) {
        Logger::writeToLog("Creating window: " + title + "...");
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
        m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, sdlFlags);
        if (!SDL_GL_CreateContext(m_window)) {
            Logger::writeErrorToLog("Failed to create a GL context: "
                                    + nta::to_string(SDL_GetError()));
        }
        glewExperimental = GL_TRUE; // I don't remember what this does or why it is needed
        
        const GLenum err = glewInit();
        if (err != GLEW_OK) {
            Logger::writeErrorToLog("Failed to initialize glew: " 
                                    + nta::to_string(glewGetErrorString(err)));
        }
        SDL_GL_SetSwapInterval(1);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        Logger::writeToLog("Created window using OpenGL version "
                           + nta::to_string(glGetString(GL_VERSION)));
    }
    void Window::swapBuffers() const {
        SDL_GL_SwapWindow(m_window);
    }
    void Window::screenshot() const {
        const int pixelArraySize = m_height*4*(m_width*3+3)/4;
        GLubyte* pixels = new GLubyte[pixelArraySize];
        glReadPixels(0, 0, m_width, m_height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
        std::stringstream imagePath;
        for (int curr = 0;; curr++) {
            imagePath.str("");
            imagePath<<"screenshots/Screenshot_"<<m_title<<"_"<<curr<<".bmp";
            std::ifstream check(imagePath.str().c_str());
            if (!check.is_open()) {
                break;
            }
        }
        std::ofstream imageFile(imagePath.str().c_str(), std::ios::binary);
        IOManager::writeShortLE(0x4D42, imageFile);
        IOManager::writeIntLE(pixelArraySize+54, imageFile);
        IOManager::writeShortLE(0, imageFile);
        IOManager::writeShortLE(0, imageFile);
        IOManager::writeIntLE(54, imageFile);
        IOManager::writeIntLE(40, imageFile);
        IOManager::writeIntLE(m_width, imageFile);
        IOManager::writeIntLE(m_height, imageFile);
        IOManager::writeShortLE(1, imageFile);
        IOManager::writeShortLE(24, imageFile);
        IOManager::writeIntLE(0, imageFile);
        IOManager::writeIntLE(pixelArraySize, imageFile);
        IOManager::writeIntLE(1, imageFile);
        IOManager::writeIntLE(1, imageFile);
        IOManager::writeIntLE(0, imageFile);
        IOManager::writeIntLE(0, imageFile);
        imageFile.write((char*)pixels, pixelArraySize);
        Logger::writeToLog("Saved screenshot");
        delete[] pixels;
    }
}
