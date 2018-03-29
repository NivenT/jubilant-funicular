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

namespace nta {
    // \todo Take parameters for gl version
    void init() {
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
        #ifdef USE_DEVIL
            ilInit();
            iluInit();
        #endif
    }
    void cleanup() {
         nta::ResourceManager::destroy();
         nta::SystemManager::destroy();
         nta::AudioManager::destroy();
         nta::CallbackManager::destroy();
         TTF_Quit();
         SDL_Quit();
    }
    glm::vec2 rotate(crvec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                         glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
    }
}
