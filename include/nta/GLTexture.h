#ifndef GLTEXTURE_H_INCLUDED
#define GLTEXTURE_H_INCLUDED

#include "MyEngine.h"

typedef unsigned int GLuint;
typedef int GLint;

namespace nta {
    class ResourceManager;
    ///represents a texture
    struct GLTexture {
        ///the id of the texture
        GLuint id;
        ///the width and height, respectively, of the texture
        GLint width, height;
    };
    ///loads images as GLTextures
    class ImageLoader {
    private:
        ///loads in any image file
        static GLTexture readImage(crstring filePath, GLint minFilt, GLint magFilt, crvec2 dimensions);
    public:
        friend class ResourceManager;
    };
}

#endif // GLTEXTURE_H_INCLUDED
