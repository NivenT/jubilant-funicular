#ifndef NTA_RESOURCEMANAGER_H_INCLUDED
#define NTA_RESOURCEMANAGER_H_INCLUDED

#include <unordered_map>

#include "nta/GLTexture.h"
#include "nta/SpriteFont.h"
#include "nta/Errors.h"

namespace nta {
    /// Handles storing and retrieving images so an image isn't loaded multiple times
    ///
    /// \todo Map from Path instead of std::string
    class ResourceManager {
    private:
        /// a map for associating a texture with the name of its file
        static std::map<std::string, RawTexture> m_textureMap;
        /// \question Why does this hold pointers?
        static std::map<std::pair<std::string, int>, SpriteFont*> m_fontMap;
    public:
        /// returns the resource with the given path, loading it if need be
        static Result<RawTexture> getTexture(crstring imagePath, 
                                             crvec2 dimensions = glm::vec2(0));
        static SpriteFont* getSpriteFont(crstring fontPath, int fontSize = 32);
        /// removes the resource with the given path from the map and deletes it
        static void deleteTexture(crstring imagePath);
        static void deleteSpriteFont(crstring fontPath, int fontSize = 32);
        static void destroy();
    };
}

#endif // NTA_RESOURCEMANAGER_H_INCLUDED
