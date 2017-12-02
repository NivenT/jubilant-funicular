#ifndef VERTEX_H_INCLUDED
#define VERTEX_H_INCLUDED

#include "MyEngine.h"

namespace nta {
    ///represents a vertex in 2 dimensions
    struct Vertex2D {
        ///Initializes an "empty" vertex
        Vertex2D(){
        }
        ///Initializes a white, textureless vertex with given position
        Vertex2D(crvec2 p) : pos(p), color(glm::vec4(1)), uv(glm::vec2(0)) {
        }
        ///Initialized a vertex with everything
        Vertex2D(crvec2 p, crvec4 c, crvec2 u) : pos(p), color(c), uv(u) {
        }
        ///sets the position of the vertex
        void setPosition(float x, float y) {
            pos.x = x;
            pos.y = y;
        }
        ///sets the color of the vertex
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
        ///sets the uv coordinates of the vertex
        void setUV(float u, float v) {
            uv.s = u;
            uv.t = v;
        }
        ///the vertex's position, color, and uv coordinates, respectively
        glm::vec2 pos;
        glm::vec4 color;
        glm::vec2 uv;
    };
}

#endif // VERTEX_H_INCLUDED
