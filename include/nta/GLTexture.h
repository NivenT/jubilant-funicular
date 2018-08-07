#ifndef NTA_GLTEXTURE_H_INCLUDED
#define NTA_GLTEXTURE_H_INCLUDED

#include "Errors.h"

typedef unsigned int GLuint;
typedef int GLint;

namespace nta {
    /// represents a texture
    struct GLTexture {
        static GLTexture no_texture() {
            GLTexture ret;
            ret.id = ret.width = ret.height = 0;
            return ret;
        }
        bool is_valid() const {
            return id != 0;
        }
        bool operator==(const GLTexture& rhs) const {
            return id == rhs.id;
        }

        /// the id of the texture
        GLuint id;
        /// the width and height, respectively, of the texture
        GLint width, height;
    };
    /// loads images as GLTextures
    class ImageLoader {
    private:
        /// loads in any image file
        static Result<GLTexture> readImage(crstring filePath, GLint minFilt, 
                                           GLint magFilt, crvec2 dimensions);
    public:
        friend class ResourceManager;
    };
}

namespace std {
  template <>
  struct hash<nta::GLTexture> {
    std::size_t operator()(const nta::GLTexture& tex) const { 
        return std::hash<GLuint>()(tex.id); 
    }
  };
}

#endif // NTA_GLTEXTURE_H_INCLUDED
