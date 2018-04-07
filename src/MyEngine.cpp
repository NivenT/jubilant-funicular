#ifdef USE_DEVIL
    #include <IL/il.h>
    #include <IL/ilu.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "nta/MyEngine.h"
#include "nta/ResourceManager.h"
#include "nta/SystemManager.h"
#include "nta/AudioManager.h"
#include "nta/CallbackManager.h"
#include "nta/Logger.h"
#include "nta/Random.h"

namespace nta {
    // \todo Take parameters for gl version
    void init() {
        Logger::createLog();
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            Logger::writeErrorToLog("Failed to initialize SDL: " + nta::to_string(SDL_GetError()));
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
        #ifdef USE_DEVIL
            ilInit();
            iluInit();
        #endif
        Random::init();
        AudioManager::init();
        CallbackManager::init();
    }
    void cleanup() {
         ResourceManager::destroy();
         SystemManager::destroy();
         AudioManager::destroy();
         CallbackManager::destroy();
         TTF_Quit();
         SDL_Quit();
    }
    glm::vec2 rotate(crvec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                         glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
    }
    bool check_error() {
        std::string gl_err;
        int err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            gl_err += gl_err == "" ? "" : "\n";
            gl_err += to_string(gluErrorString(err)) + " (" + to_string(err) + ")";
        }
        if (gl_err != "") {
            Logger::writeToLog("GL Error(s): " + gl_err);
        }
        
        std::string sdl_err = SDL_GetError();
        if (sdl_err != "") {
            Logger::writeToLog("SDL Error: "+sdl_err);
            SDL_ClearError();
        }
        return err != GL_NO_ERROR || sdl_err != "";
    }
}
