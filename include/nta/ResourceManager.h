#ifndef NTA_RESOURCEMANAGER_H_INCLUDED
#define NTA_RESOURCEMANAGER_H_INCLUDED

#include <SDL2/SDL_ttf.h>

#include "nta/GLTexture.h"
#include "nta/SpriteFont.h"
#include "nta/Errors.h"

namespace nta {
    /// Responsible for loading and caching external files, so they are only
    /// loaded in once during program execution. In general, you you probably
    /// don't want to use ResourceManager directly, but instead use ContextData.
    ///
    /// \todo Map from Path instead of std::string
    class ResourceManager {
    private:
        /// a map for associating a texture with the name of its file
        static std::map<std::string, RawTexture> m_textureMap;
        static std::map<std::pair<std::string, int>, TTF_Font*> m_fontMap;
    public:
        /// returns the resource with the given path, loading it if need be
        static Result<RawTexture> getTexture(crstring imagePath, 
                                             crvec2 dimensions = glm::vec2(0));
        static Result<TTF_Font*> getFont(crstring fontPath, int fontSize = 32);
        static void destroy();
    };
}

#endif // NTA_RESOURCEMANAGER_H_INCLUDED
