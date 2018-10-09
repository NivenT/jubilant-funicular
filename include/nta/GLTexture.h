#ifndef NTA_GLTEXTURE_H_INCLUDED
#define NTA_GLTEXTURE_H_INCLUDED

#include <GL/glew.h>

#include "Errors.h"

namespace nta {
    /// A loaded image (not tied to any specific GL context)
    ///
    /// During program execution, no two RawTexture objects should
    /// represent the same image unless they have the same data pointer.
    struct RawTexture {
        bool operator==(const RawTexture& rhs) const {
            return data == rhs.data;
        }

        GLuint width, height;
        GLint format;
        /// Data stored on the heap
        GLubyte* data = nullptr;
    };
    /// represents a texture (tied to a specific GL context)
    struct GLTexture {
        GLTexture() : id(0) {}
        GLTexture(const RawTexture& raw, GLint minFilt = GL_LINEAR_MIPMAP_LINEAR,
                  GLint magFilt = GL_LINEAR);
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
        static Result<RawTexture> readImage(crstring filePath, crvec2 dimensions);
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
