#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <GL/glew.h>

#include "GLTexture.h"

namespace nta {
    ///represents a textured quad
    class Sprite {
    private:
        ///id for the vertex buffer object used to render the sprite
        GLuint m_vboID;
        ///texture used by the quad
        GLTexture m_texture;
    public:
        ///constructor and destructor
        Sprite();
        ~Sprite();
        ///creates the sprite
        void init(float x, float y, float w, float h, crstring imagePath, float d = 0);
        ///renders the sprite
        void render() const;
    };
}

#endif // SPRITE_H_INCLUDED
