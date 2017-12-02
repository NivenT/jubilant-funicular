#include <algorithm>

#include "PrimitiveBatch.h"

namespace nta {
    PrimitiveBatch::PrimitiveBatch() {
    }
    PrimitiveBatch::~PrimitiveBatch() {
        if (m_vbo != 0) {
            glDeleteBuffers(1, &m_vbo);
        }
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }
        for (int i = 0; i < m_primitives.size(); i++) {
            delete m_primitives[i];
        }
        m_primitives.clear();
        m_renderBatches.clear();
    }
    void PrimitiveBatch::init() {
        createVertexArrayObject();
    }
    void PrimitiveBatch::createVertexArrayObject() {
        if (m_vbo == 0) {
            glGenBuffers(1, &m_vbo);
        }
        if (m_vao == 0) {
            glGenVertexArrays(1, &m_vao);
        }
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        for (int i = 0; i < 3; i++) {
            glEnableVertexAttribArray(i);
        }
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, pos));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE , sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));
        glBindVertexArray(0);
    }
    void PrimitiveBatch::begin() {
        for (int i = 0; i < m_primitives.size(); i++) {
            delete m_primitives[i];
        }
        m_primitives.clear();
        m_renderBatches.clear();
    }
    void PrimitiveBatch::end() {
        sortPrimitives();
        createRenderBatches();
    }
    void PrimitiveBatch::sortPrimitives() {
        //sort overall by depth
        std::stable_sort(m_primitives.begin(), m_primitives.end(), compareDepth);
        //sort same depth subsets by primitive
        int begin = 0;
        for (int i = 1; i < m_primitives.size(); i++) {
            if (m_primitives[i]->depth != m_primitives[i-1]->depth) {
                std::stable_sort(m_primitives.begin()+begin, m_primitives.begin()+i, comparePrimitive);
                begin = i;
            }
        }
        //sort same depth and primitive subsets by texture
        begin = 0;
        for (int i = 1; i < m_primitives.size(); i++) {
            if (m_primitives[i]->depth != m_primitives[i-1]->depth ||
                m_primitives[i]->vertices.size() != m_primitives[i-1]->vertices.size()) {
                std::stable_sort(m_primitives.begin()+begin, m_primitives.begin()+i, compareTexture);
                begin = i;
            }
        }
    }
    bool PrimitiveBatch::compareDepth(Primitive* lhs, Primitive* rhs) {
        return (lhs->depth < rhs->depth);
    }
    bool PrimitiveBatch::compareTexture(Primitive* lhs, Primitive* rhs) {
        return (lhs->textureID < rhs->textureID);
    }
    bool PrimitiveBatch::comparePrimitive(Primitive* lhs, Primitive* rhs) {
        return (lhs->vertices.size() < rhs->vertices.size());
    }
    void PrimitiveBatch::createRenderBatches() {
        if (m_primitives.empty()) {
            return;
        }
        int numVertices = 0; //total number of vertices
        std::for_each(m_primitives.begin(), m_primitives.end(), [&numVertices](const Primitive* p) {
           numVertices += p->vertices.size();
        });
        Vertex2D vertexData[numVertices];
        m_renderBatches.emplace_back(m_primitives[0]->textureID, 0, m_primitives[0]->vertices.size(),
                                    toPrimitiveType(m_primitives[0]->vertices.size()));
        int cv = 0; ///current vertex
        for (int i = 0; i < m_primitives[0]->vertices.size(); i++) {
            vertexData[cv++] = m_primitives[0]->vertices[i];
        }
        int offset = m_primitives[0]->vertices.size();
        for (int cp = 1; cp < m_primitives.size(); cp++) { ///current primitive
            if (m_primitives[cp]->textureID != m_primitives[cp-1]->textureID ||
                m_primitives[cp]->vertices.size() != m_primitives[cp-1]->vertices.size()) {
                m_renderBatches.emplace_back(m_primitives[cp]->textureID, offset, m_primitives[cp]->vertices.size(),
                                            toPrimitiveType(m_primitives[cp]->vertices.size()));
            } else {
                m_renderBatches.back().numVertices += m_primitives[cp]->vertices.size();
            }
            for (int i = 0; i < m_primitives[cp]->vertices.size(); i++) {
                vertexData[cv++] = m_primitives[cp]->vertices[i];
            }
            offset += m_primitives[cp]->vertices.size();
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexData), vertexData);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    GLenum PrimitiveBatch::toPrimitiveType(unsigned int numVertices) const {
        switch(numVertices) {
        case 1: return GL_POINTS; break;
        case 2: return GL_LINES; break;
        case 3: return GL_TRIANGLES; break;
        case 4: return GL_QUADS; break;
        default: return GL_POLYGON; break;
        }
    }
    void PrimitiveBatch::addPrimitive(Primitive* primitive) {
        m_primitives.push_back(primitive);
    }
    void PrimitiveBatch::addPrimitive(const std::initializer_list<Vertex2D>& vertices, GLuint textureID, float depth) {
        m_primitives.push_back(new Primitive(vertices, textureID, depth));
    }
    void PrimitiveBatch::render() const {
        glBindVertexArray(m_vao);
        for (int i = 0; i < m_renderBatches.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].textureID);
            glDrawArrays(m_renderBatches[i].mode, m_renderBatches[i].offset, m_renderBatches[i].numVertices);
        }
        glBindVertexArray(0);
    }
}
