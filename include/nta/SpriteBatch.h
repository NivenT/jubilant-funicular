#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include <GL/glew.h>
#include <vector>

#include "Vertex.h"

namespace nta {
    ///represents what is essentially a sprite
    struct Glyph {
                                    Glyph(crvec4 posRect, crvec4 uvRect, GLuint texture, float d, crvec4 color) : textureID(texture), depth(d) {
                                        topLeft.setPosition(posRect.x, posRect.y);
                                        topLeft.setUV(uvRect.x, uvRect.y);
                                        topLeft.color = color;
                                        topRight.setPosition(posRect.x + posRect[2], posRect.y);
                                        topRight.setUV(uvRect.x + uvRect[2], uvRect.y);
                                        topRight.color = color;
                                        botRight.setPosition(posRect.x + posRect[2], posRect.y - posRect[3]);
                                        botRight.setUV(uvRect.x + uvRect[2], uvRect.y + uvRect[3]);
                                        botRight.color = color;
                                        botLeft.setPosition(posRect.x, posRect.y - posRect[3]);
                                        botLeft.setUV(uvRect.x, uvRect.y + uvRect[3]);
                                        botLeft.color = color;
                                    }
        ///the texture used by the glyph
        GLuint                      textureID;
        ///the depth of the glyph
        float                       depth;
        ///the vertices of the four corners of the glyph
        Vertex2D                    topLeft;
        Vertex2D                    topRight;
        Vertex2D                    botRight;
        Vertex2D                    botLeft;
    };
    ///stores information about batches of vertices with the same texture in a vertex buffer object
    struct RenderBatch {
        ///constructor
                                    RenderBatch(GLuint t, GLuint o, GLuint n, GLenum m = GL_TRIANGLES) : textureID(t), offset(o), numVertices(n), mode(m) {
                                    }
        ///the texture used by the batch
        GLuint                      textureID;
        ///the starting point of the batch in the vertex buffer
        GLuint                      offset;
        ///the number of vertices comprising the vertex buffer
        GLuint                      numVertices;
        ///the primitive type to be drawn (GL_POINTS, GL_LINES, etc.)
        GLenum                      mode;
    };
    ///represents a collection of sprites to be drawn
    class SpriteBatch {
    private:
        ///creates the vertex array object
        void                        createVertexArrayObject();
        ///creates the render batches
        void                        createRenderBatches();
        ///sorts the glyphs
        void                        sortGlyphs();
        ///comparers used to sort
        static bool                 compareTexture(Glyph* lhs, Glyph* rhs);
        static bool                 compareDepth(Glyph* lhs, Glyph* rhs);
        ///pointers to the glyphs to be rendered (used for sorting)
        std::vector<Glyph*>         m_glyphPointers;
        ///the glyphs to be rendered (used for fast vector operations)
        std::vector<Glyph>          m_glyphs;
        ///the render batches used for rendering the glyphs
        std::vector<RenderBatch>    m_renderBatches;
        ///ids of the vertex buffer object and vertex array object used to render the glyphs
        GLuint                      m_vao = 0;
        GLuint                      m_vbo = 0;
    public:
        ///constructor and destructor
                                    SpriteBatch();
                                    ~SpriteBatch();
        ///initializes the batch
        void                        init();
        ///begins collection of glyphs for the batch
        void                        begin();
        ///ends collection of glyphs and prepares to render
        void                        end();
        ///adds a glyph to the batch
        void                        addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, float depth = 1,
                                             crvec4 color = glm::vec4(1));
        void                        addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect,
                                             GLuint texture, float depth = 1, crvec4 color = glm::vec4(1));
        ///renders the batch
        void                        render() const;
    };
}

#endif // SPRITEBATCH_H
