#include <SDL2/SDL.h>
#include <functional>

namespace nta {
    void init() {
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
    }
}
/*
float changeRange(float val, float a, float b, float c, float d) {
    ///[a,b]->[0,b-a]->[0,d-c]->[c,d]
    return (val-a)*(d-c)/(b-a) + c;
}
///Calls the first function corresponding to a true statement
void cond(std::initializer_list<bool> conditions, std::initializer_list<std::function<void()>> exprs) {
    for (int i = 0; i < conditions.size(); i++) {
        if (*(conditions.begin()+i)) {
            (*(exprs.begin()+i))();
        }
    }
}
*/
