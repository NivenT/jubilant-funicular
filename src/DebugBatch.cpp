#include "nta/DebugBatch.h"
#include "nta/PrimitiveBatch.h"

namespace nta {
    /*
    glm::vec2 rotate(glm::vec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                         glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
    }
    */
    DebugBatch::DebugBatch() {
    }
    DebugBatch::~DebugBatch() {
        if (m_ibo != 0) glDeleteBuffers(1, &m_ibo);
        if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
        if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
        m_ibo = m_vbo = m_vao = 0;

        m_vertices.clear();
        m_indices.clear();
    }
    void DebugBatch::init() {
        if (m_ibo == 0) glGenBuffers(1, &m_ibo);
        if (m_vbo == 0) glGenBuffers(1, &m_vbo);
        if (m_vao == 0) glGenVertexArrays(1, &m_vao);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

        for (int i = 0; i < NUM_VERTEX_ATTRIBS; i++) {
            auto& attrib = Vertex2D::attribs[i];

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, attrib.size, attrib.type, attrib.normalized, sizeof(Vertex2D), 
                                  attrib.pointer);
        }
        glBindVertexArray(0);
    }
    void DebugBatch::begin() {
        m_vertices.clear();
        m_indices.clear();
    }
    void DebugBatch::end() {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D)*m_vertices.size(), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D)*m_vertices.size(), m_vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_indices.size(), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint)*m_indices.size(), m_indices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void DebugBatch::addLine(crvec2 start, crvec2 end, crvec4 color, int num_pieces) {
        const int start_index = m_vertices.size();
        const glm::vec2 diff = end - start;
        for (int i = 0; i <= num_pieces; i++) {
            m_vertices.emplace_back(start + (float)i*diff/(float)num_pieces, color);
            if (i > 0) {
                m_indices.push_back(start_index + i - 1);
                m_indices.push_back(start_index + i);
            }
        }
    }
    void DebugBatch::addLine(crvec2 start, crvec2 end, int num_pieces, crvec4 color) {
        addLine(start, end, color, num_pieces);
    }
    void DebugBatch::addRect(crvec4 posRect, crvec4 color, float orientation) {
        const int start = m_vertices.size();
        glm::vec2 half_dims(posRect[2]/2, posRect[3]/2);
        glm::vec2 center(posRect.x + half_dims.x, posRect.y - half_dims.y);

        glm::vec2 tl = center + glm::vec2(-half_dims.x, half_dims.y),
                  tr = center + half_dims,
                  br = center - glm::vec2(-half_dims.x, half_dims.y),
                  bl = center - half_dims;

        m_vertices.emplace_back(center + rotate(tl, orientation), color);
        m_vertices.emplace_back(center + rotate(tr, orientation), color);
        m_vertices.emplace_back(center + rotate(br, orientation), color);
        m_vertices.emplace_back(center + rotate(bl, orientation), color);

        for (int offset = 0; offset < 4; offset++) {
            m_indices.push_back(start + offset);
            m_indices.push_back(start + (offset + 1)%4);
        }
    }
    void DebugBatch::addRect(crvec4 posRect, float orientation, crvec4 color) {
        addRect(posRect, color, orientation);
    }
    void DebugBatch::addCircle(crvec2 center, float radius, crvec4 color) {
        static const std::size_t NUM_SIDES = 100;
        float side_length = radius * glm::sqrt(2.-2.*glm::cos(2.*M_PI/NUM_SIDES));
        addPolygon(NUM_SIDES, center, side_length, 0., color);
    }
    void DebugBatch::addPolygon(std::size_t numSides, crvec2 center, float sideLength,
                                float orientation, crvec4 color) {
        const int start = m_vertices.size();

        Primitive poly(numSides, center, sideLength, color, orientation, NTA_DEFAULT_DEPTH);
        m_vertices.insert(m_vertices.end(), std::begin(poly.vertices), std::end(poly.vertices));

        for (int offset = 0; offset < poly.vertices.size(); offset++) {
            m_indices.push_back(start + offset);
            m_indices.push_back(start + (offset + 1)%poly.vertices.size());
        }
    }
    void DebugBatch::render() const {
        glBindVertexArray(m_vao);
        glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}