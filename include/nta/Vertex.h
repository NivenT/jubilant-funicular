#ifndef NTA_VERTEX_H_INCLUDED
#define NTA_VERTEX_H_INCLUDED

#include <GL/glew.h>

#include "nta/MyEngine.h"

#define NUM_VERTEX_ATTRIBS 4

/// \todo Have first-class support for arbitrary (i.e. user-defined) vertex types
namespace nta {
    /// represents an attribute of a vertex (e.g. a call to glVertexAttribPointer)
    struct VertexAttrib {
        void setup() const {
            glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        }

        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        GLvoid* pointer;
    };
    /// represents a vertex in 2 dimensions
    struct Vertex2D {
        /// Initializes an "empty" vertex
        Vertex2D(){
        }
        /// Initializes a white, textureless vertex with given position
        Vertex2D(crvec2 p) : pos(p), color(glm::vec4(1)), uv(glm::vec2(0)), hasTexture(0.0) {
        }
        /// Initializes textureless, colorful vertex
        Vertex2D(crvec2 p, crvec4 c) : pos(p), color(c), uv(glm::vec2(0)), hasTexture(0.0) {
        }
        /// Initializes a vertex with everything
        Vertex2D(crvec2 p, crvec4 c, crvec2 u, float t = 1.0) : pos(p), color(c), uv(u), hasTexture(t) {
        }
        /// Initializes vertex with everything except position
        Vertex2D(crvec4 c, crvec2 u, float t = 1.0) : pos(0), color(c), uv(u), hasTexture(t) {
        }
        /// sets the position of the vertex
        void setPosition(float x, float y) {
            pos.x = x;
            pos.y = y;
        }
        /// sets the color of the vertex
        void setColor(float r, float g, float b, float a) {
            color.r = r;
            color.g = g;
            color.b = b;
            color.a = a;
        }
        void setColor(crvec3 c) {
            color.r = c.r;
            color.g = c.g;
            color.b = c.b;
            color.a = 1;
        }
        /// sets the uv coordinates of the vertex
        void setUV(float u, float v) {
            uv.s = u;
            uv.t = v;
        }
        /// the vertex's position, color, and uv coordinates, respectively
        glm::vec2 pos;
        glm::vec4 color;
        glm::vec2 uv;
        float hasTexture;

        static const VertexAttrib attribs[NUM_VERTEX_ATTRIBS];
    };
}

#endif // NTA_VERTEX_H_INCLUDED
