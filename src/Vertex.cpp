#include "nta/Vertex.h"

/// \todo Figure out how to get rid of this file
namespace nta {
    const VertexAttrib Vertex2D::attribs[NUM_VERTEX_ATTRIBS] = {
        {2, GL_FLOAT, GL_FALSE, (void*)offsetof(Vertex2D, pos)},
        {4, GL_FLOAT, GL_TRUE,  (void*)offsetof(Vertex2D, color)},
        {2, GL_FLOAT, GL_FALSE, (void*)offsetof(Vertex2D, uv)},
        {1, GL_FLOAT, GL_FALSE, (void*)offsetof(Vertex2D, hasTexture)}
    };
}