#ifndef NTA_SPRITEBATCH_H_INCLUDED
#define NTA_SPRITEBATCH_H_INCLUDED

#include <vector>

#include "nta/Vertex.h"
#include "nta/utils.h"

#define NTA_DEFAULT_DEPTH 0.5

namespace nta {
    /// represents what is essentially a sprite
    struct Glyph {
        Glyph(crvec4 posRect, crvec4 uvRect, GLuint texture, float d, crvec4 color) :
            textureID(texture), depth(d) {
            topLeft.setPosition(posRect.x, posRect.y);
            topLeft.setUV(uvRect.x, uvRect.y);
            topLeft.color = color;
            topLeft.hasTexture = texture != 0;
            topRight.setPosition(posRect.x + posRect[2], posRect.y);
            topRight.setUV(uvRect.x + uvRect[2], uvRect.y);
            topRight.color = color;
            topRight.hasTexture = texture != 0;
            botRight.setPosition(posRect.x + posRect[2], posRect.y - posRect[3]);
            botRight.setUV(uvRect.x + uvRect[2], uvRect.y + uvRect[3]);
            botRight.color = color;
            botRight.hasTexture = texture != 0;
            botLeft.setPosition(posRect.x, posRect.y - posRect[3]);
            botLeft.setUV(uvRect.x, uvRect.y + uvRect[3]);
            botLeft.color = color;
            botLeft.hasTexture = texture != 0;
        }
        /// \todo Use model matrix?
        Glyph(crvec4 posRect, crvec4 uvRect, GLuint tex, float d, crvec4 col, float angle) :
            textureID(tex), depth(d) {
            glm::vec2 extents(posRect[2]/2.f, posRect[3]/2.f), pos(posRect.x, posRect.y);
            glm::vec2 tl(-extents.x, extents.y),  tr(extents.x, extents.y),
                      bl(-extents.x, -extents.y), br(extents.x, -extents.y);

            topLeft.pos = utils::rotate(tl, angle) - tl + pos;
            topLeft.setUV(uvRect.x, uvRect.y);
            topLeft.color = col;
            topLeft.hasTexture = tex != 0;
            topRight.pos = utils::rotate(tr, angle) - tl + pos;
            topRight.setUV(uvRect.x + uvRect[2], uvRect.y);
            topRight.color = col;
            topRight.hasTexture = tex != 0;
            botRight.pos = utils::rotate(br, angle) - tl + pos;
            botRight.setUV(uvRect.x + uvRect[2], uvRect.y + uvRect[3]);
            botRight.color = col;
            botRight.hasTexture = tex != 0;
            botLeft.pos = utils::rotate(bl, angle) - tl + pos;
            botLeft.setUV(uvRect.x, uvRect.y + uvRect[3]);
            botLeft.color = col;
            botLeft.hasTexture = tex != 0;
        }
        /// the texture used by the glyph
        GLuint textureID;
        /// the depth of the glyph
        float depth;
        /// the vertices of the four corners of the glyph
        Vertex2D topLeft;
        Vertex2D topRight;
        Vertex2D botRight;
        Vertex2D botLeft;
    };
    /// stores information about batches of vertices with the same texture in a vertex buffer object
    struct RenderBatch {
        /// constructor
        RenderBatch(GLuint t, GLuint o, GLuint n, GLenum m = GL_TRIANGLES) :
            textureID(t), offset(o), numVertices(n), mode(m) {
        }
        /// the texture used by the batch
        GLuint textureID;
        /// the starting point of the batch in the vertex buffer
        GLuint offset;
        /// the number of vertices comprising the vertex buffer
        GLuint numVertices;
        /// the primitive type to be drawn (GL_POINTS, GL_LINES, etc.)
        GLenum mode;
    };
    /// represents a collection of sprites to be drawn
    ///
    /// If you want to draw a shape other than a square, use PrimitiveBatch instead
    /// \todo Allow for custom vertex types
    /// \todo Rewrite to use glDrawElements instead of glDrawArrays
    class SpriteBatch {
    private:
        /// creates the vertex array object
        void createVertexArrayObject();
        /// creates the render batches
        void createRenderBatches();
        /// sorts the glyphs
        void sortGlyphs();
        /// comparers used to sort
        static bool compareTexture(Glyph* lhs, Glyph* rhs);
        static bool compareDepth(Glyph* lhs, Glyph* rhs);
        /// pointers to the glyphs to be rendered (used for sorting)
        std::vector<Glyph*> m_glyphPointers;
        /// the glyphs to be rendered (used for fast vector operations)
        std::vector<Glyph> m_glyphs;
        /// the render batches used for rendering the glyphs
        std::vector<RenderBatch> m_renderBatches;
        /// ids of the vertex buffer object and vertex array object used to render the glyphs
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
    public:
        /// constructor and destructor
        SpriteBatch();
        ~SpriteBatch();
        /// initializes the batch
        void init();
        /// begins collection of glyphs for the batch
        void begin();
        /// ends collection of glyphs and prepares to render
        void end();
        /// adds a glyph to the batch
        /// \todo Give some of these different names?
        void addGlyph(crvec4 posRect, GLuint texture = 0, 
                      crvec4 uvRect = glm::vec4(0,0,1,1), 
                      crvec4 color = glm::vec4(1),
                      float angle = 0,
                      float depth = NTA_DEFAULT_DEPTH);
        void addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, 
                      float depth = NTA_DEFAULT_DEPTH, crvec4 color = glm::vec4(1));
        void addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect, GLuint texture, 
                      float depth = NTA_DEFAULT_DEPTH, crvec4 color = glm::vec4(1));
        void addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, crvec4 color, 
                      float angle = 0.0f, float depth = NTA_DEFAULT_DEPTH);
        void addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect, GLuint texture, crvec4 color,
                      float angle = 0.0f, float depth = NTA_DEFAULT_DEPTH);
        /// renders the batch
        void render() const;
    };
}

#endif // NTA_SPRITEBATCH_H_INCLUDED
