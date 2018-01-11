#ifndef DEBUGBATCH_H_INCLUDED
#define DEBUGBATCH_H_INCLUDED

#include <GL/glew.h>
#include <vector>

#include "nta/Vertex.h"

// TODO: Make abstract RenderBatch class for this, SpriteBatch, and PrimitiveBatch
namespace nta {
    /// A collection of debug primitives (outlines of shapes) to be drawn
    class DebugBatch {
    private:
        std::vector<Vertex2D> m_vertices;
        std::vector<GLuint> m_indices;
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ibo = 0;
    public:
        /// Constructor. Doesn't actually do anything; use init() to initialize instead
        DebugBatch();
        ~DebugBatch();
        /// initializes the batch (only needs to be called once)
        void init();
        /// begins collecting shapes to be drawn
        void begin();
        /// ends collecting shapes
        void end();
        // TODO: Learn how to have doxygen use one comment for multiple functions
        /// Adds a shape to the batch
        void addLine(crvec2 start, crvec2 end, crvec4 color = glm::vec4(1), int num_pieces = 2);
        void addLine(crvec2 start, crvec2 end, int num_pieces, crvec4 color = glm::vec4(1));
        void addRect(crvec4 posRect, crvec4 color = glm::vec4(1), float orientation = 0);
        void addRect(crvec4 posRect, float orientation, crvec4 color = glm::vec4(1));
        void addCircle(crvec2 center, float radius, crvec4 color = glm::vec4(1));
        void addPolygon(std::size_t numSides, crvec2 center, float sideLength, float orientation, 
                        crvec4 color = glm::vec4(1));
        /// renders the batch
        void render() const;
    };
}

#endif // DEBUGBATCH_H_INCLUDED