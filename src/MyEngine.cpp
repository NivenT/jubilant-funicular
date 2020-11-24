#include <memory>
#include <mutex>

#ifdef NTA_USE_DEVIL
    #include <IL/il.h>
    #include <IL/ilu.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef NTA_USE_IMGUI
    #include <imgui/imgui.h>
    #include <imgui/imgui_impl_sdl.h>
    #include <imgui/imgui_impl_opengl3.h>
#endif

#include "nta/MyEngine.h"
#include "nta/ResourceManager.h"
#include "nta/WindowManager.h"
#include "nta/CallbackManager.h"
#include "nta/Logger.h"
#include "nta/Random.h"
#include "nta/utils.h"

#ifdef NTA_USE_AUDIO
    #include "nta/AudioManager.h"
#endif

// This might be surprising if you look at most of the code I write, but I
// do know about smart pointers
static std::map<std::ostream*, std::unique_ptr<std::mutex>> g_stream_locks;
static std::mutex g_map_lock;

namespace nta {
    void init(int gl_major_version, int gl_minor_version, bool use_gl_core) {
        Logger::createLog();
        Logger::writeToLog("Initializing Engine...");
        Logger::indent();
        Logger::writeToLog("Initializing SDL2...");
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            Logger::writeErrorToLog("Failed to initialize SDL: " + utils::to_string(SDL_GetError()),
                                    SDL_FAILURE);
        }
        if (gl_major_version > 0) SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
        if (gl_minor_version >= 0) SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);
        if (use_gl_core) SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
        TTF_Init();
        #ifdef NTA_USE_IMGUI
            Logger::writeToLog("Initializing imgui...");
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
        #endif
        #ifdef NTA_USE_DEVIL
            Logger::writeToLog("Initializing DevIL");
            ilInit();
            iluInit();
        #endif
        Random::init();
        #ifdef NTA_USE_AUDIO
            AudioManager::init();
        #endif
        //CallbackManager::init();
        Logger::unindent();
        Logger::writeToLog("Initialized Engine");
    }
    void cleanup() {
        #ifdef NTA_USE_IMGUI
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
        #endif
        ResourceManager::destroy();
        WindowManager::destroy();
        #ifdef NTA_USE_AUDIO
            AudioManager::destroy();
        #endif
        //CallbackManager::destroy();
        TTF_Quit();
        SDL_Quit();
    }
    bool check_error() {
        std::string gl_err;
        int err = GL_NO_ERROR;
        if (SDL_GL_GetCurrentContext() != nullptr) {
            while ((err = glGetError()) != GL_NO_ERROR) {
                gl_err += gl_err == "" ? "" : "\n";
                gl_err += utils::to_string(gluErrorString(err)) + " (" + utils::to_string(err) + ")";
            }
            if (gl_err != "") {
                Logger::writeToLog("GL Error(s): {}", gl_err);
            }
        }
        std::string sdl_err = SDL_GetError();
        if (sdl_err != "") {
            Logger::writeToLog("SDL Error: {}", sdl_err);
            SDL_ClearError();
        }
        return err != GL_NO_ERROR || sdl_err != "";
    }
    std::ostream& lock_stream(std::ostream& stream) {
        g_map_lock.lock();
        auto& lock = g_stream_locks[&stream];
        if (!lock) lock.reset(new std::mutex);
        g_map_lock.unlock();
        lock->lock();
        return stream;
    }
    std::ostream& unlock_stream(std::ostream& stream) {
        g_map_lock.lock();
        auto& lock = g_stream_locks[&stream];
        g_map_lock.unlock();
        if (lock) lock->unlock();
        return stream;
    }
}
