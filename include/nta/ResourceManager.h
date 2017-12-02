#ifndef RESOURCEMANAGER_H_INCLUDED
#define RESOURCEMANAGER_H_INCLUDED

#include "GLTexture.h"
#include "SpriteFont.h"

namespace nta {
    ///Handles storing and retrieving textures so an image isn't loaded multiple times
    class ResourceManager {
    private:
        ///a map for associating a texture with the name of its file
        static std::map<std::string, GLTexture>     m_textureMap;
        static std::map<std::pair<std::string, int>,
                        SpriteFont*>                m_fontMap;
    public:
        ///returns the resource with the given path, loading it if need be
        static GLTexture&                           getTexture(crstring imagePath, GLint minFilt = GL_LINEAR_MIPMAP_LINEAR,
                                                               GLint magFilt = GL_LINEAR, crvec2 dimensions = glm::vec2(0));
        static GLTexture&                           getTexture(crstring imagePath, crvec2 dimensions,
                                                               GLint minFilt = GL_LINEAR_MIPMAP_LINEAR, GLint magFilt = GL_LINEAR) {
                                                                    return getTexture(imagePath, minFilt, magFilt, dimensions);
                                                                }
        static SpriteFont*                          getSpriteFont(crstring fontPath, int fontSize = 32);
        ///removes the resource with the given path from the map and deletes it
        static void                                 deleteTexture(crstring imagePath);
        static void                                 deleteSpriteFont(crstring fontPath, int fontSize = 32);
        static void                                 destroy();
    };
}

#endif // RESOURCEMANAGER_H_INCLUDED
