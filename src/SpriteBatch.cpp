#include <algorithm>

#include "nta/SpriteBatch.h"
#include "nta/Vertex.h"

namespace nta {
    SpriteBatch::SpriteBatch() {
    }
    SpriteBatch::~SpriteBatch() {
        if (m_vbo != 0) {
            glDeleteBuffers(1, &m_vbo);
        }
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }
        m_glyphs.clear();
        m_glyphPointers.clear();
        m_renderBatches.clear();
    }
    void SpriteBatch::init() {
        createVertexArrayObject();
    }
    void SpriteBatch::createVertexArrayObject() {
        if (m_vbo == 0) {
            glGenBuffers(1, &m_vbo);
        }
        if (m_vao == 0) {
            glGenVertexArrays(1, &m_vao);
        }
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        for (int i = 0; i < NUM_VERTEX_ATTRIBS; i++) {
            auto& attrib = Vertex2D::attribs[i];

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, attrib.size, attrib.type, attrib.normalized, sizeof(Vertex2D), 
                                  attrib.pointer);
        }
        glBindVertexArray(0);
    }
    void SpriteBatch::begin() {
        m_glyphs.clear();
        m_renderBatches.clear();
    }
    void SpriteBatch::end() {
        m_glyphPointers.resize(m_glyphs.size());
        for (int i = 0; i < m_glyphs.size(); i++) {
            m_glyphPointers[i] = &m_glyphs[i];
        }
        sortGlyphs();
        createRenderBatches();
    }
    void SpriteBatch::sortGlyphs() {
        std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareDepth);
        int begin = 0;
        for (int i = 1; i < m_glyphPointers.size(); i++) {
            if (m_glyphPointers[i]->depth != m_glyphPointers[i-1]->depth) {
                std::stable_sort(m_glyphPointers.begin()+begin, m_glyphPointers.begin()+i, compareTexture);
                begin = i;
            }
        }
    }
    bool SpriteBatch::compareDepth(Glyph* lhs, Glyph* rhs) {
        return (lhs->depth < rhs->depth);
    }
    bool SpriteBatch::compareTexture(Glyph* lhs, Glyph* rhs) {
        return (lhs->textureID < rhs->textureID);
    }
    void SpriteBatch::createRenderBatches() {
        if (m_glyphPointers.empty()) {
            return;
        }

        std::vector<Vertex2D> vertexData(6*m_glyphPointers.size());
        m_renderBatches.emplace_back(m_glyphPointers[0]->textureID, 0, 6);
        int cv = 0; // current vertex
        vertexData[cv++] = m_glyphPointers[0]->topLeft;
        vertexData[cv++] = m_glyphPointers[0]->topRight;
        vertexData[cv++] = m_glyphPointers[0]->botLeft;
        vertexData[cv++] = m_glyphPointers[0]->topRight;
        vertexData[cv++] = m_glyphPointers[0]->botLeft;
        vertexData[cv++] = m_glyphPointers[0]->botRight;
        int offset = 6;
        for (int cg = 1; cg < m_glyphPointers.size(); cg++) { // current glyph
            if (m_glyphPointers[cg]->textureID != m_glyphPointers[cg-1]->textureID) {
                m_renderBatches.emplace_back(m_glyphPointers[cg]->textureID, offset, 6);
            } else {
                m_renderBatches.back().numVertices += 6;
            }
            vertexData[cv++] = m_glyphPointers[cg]->topLeft;
            vertexData[cv++] = m_glyphPointers[cg]->topRight;
            vertexData[cv++] = m_glyphPointers[cg]->botLeft;
            vertexData[cv++] = m_glyphPointers[cg]->topRight;
            vertexData[cv++] = m_glyphPointers[cg]->botLeft;
            vertexData[cv++] = m_glyphPointers[cg]->botRight;
            offset += 6;
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size()*sizeof(Vertex2D), vertexData.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void SpriteBatch::addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, float depth,
                               crvec4 color) {
        addGlyph(posRect, uvRect, texture, color, 0, depth);
    }
    void SpriteBatch::addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect, GLuint texture,
                               float depth, crvec4 color) {
        addGlyph(corner1, corner2, uvRect, texture, color, 0, depth);
    }
    /*
    void SpriteBatch::addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, crvec4 color,
                               float depth) {
        addGlyph(posRect, uvRect, texture, depth, color);
    }
    void SpriteBatch::addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect, GLuint texture, 
                               crvec4 color, float depth) {
        addGlyph(corner1, corner2, uvRect, texture, depth, color);
    }
    */
    void SpriteBatch::addGlyph(crvec4 posRect, crvec4 uvRect, GLuint texture, crvec4 color,
                               float angle, float depth) {
        m_glyphs.emplace_back(posRect, uvRect, texture, depth, color, angle);
    }
    void SpriteBatch::addGlyph(crvec2 corner1, crvec2 corner2, crvec4 uvRect, GLuint texture, 
                               crvec4 color, float angle, float depth) {
        glm::vec4 posRect(glm::min(corner1.x, corner2.x), glm::max(corner1.y, corner2.y), 
                          glm::abs(corner1-corner2));
        addGlyph(posRect, uvRect, texture, color, angle, depth);
    }
    void SpriteBatch::render() const {
        glBindVertexArray(m_vao);
        for (int i = 0; i < m_renderBatches.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, m_renderBatches[i].textureID);
            glDrawArrays(m_renderBatches[i].mode, m_renderBatches[i].offset, 
                         m_renderBatches[i].numVertices);
        }
        glBindVertexArray(0);
    }
}
