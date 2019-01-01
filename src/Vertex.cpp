#include "nta/Vertex.h"

/// \todo Figure out how to get rid of this file
namespace nta {
    const VertexAttrib Vertex2D::attribs[NUM_VERTEX_ATTRIBS] = {
        {0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, pos)},
        {1, 4, GL_FLOAT, GL_TRUE,  sizeof(Vertex2D), (void*)offsetof(Vertex2D, color)},
        {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv)},
        {3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, hasTexture)}
    };
}
