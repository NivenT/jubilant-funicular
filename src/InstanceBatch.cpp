#include "nta/InstanceBatch.h"

namespace nta {
    void InstanceBatch::begin() {
        for (auto& glyph : m_glyphs) {
            if (!glyph.vbos.empty()) {
                glDeleteBuffers(glyph.vbos.size(), &glyph.vbos[0]);
            }
            glDeleteVertexArrays(1, &glyph.vao);
        }
        m_glyphs.clear();
    }
    void InstanceBatch::add_glyph(crvec4 posRect, GLuint texture, crvec4 uvRect, 
                                  crvec4 color) {
        m_glyphs.emplace_back(texture);
        GLuint vbo = m_glyphs.back().gen_vbo();

        glBindVertexArray(m_glyphs.back().vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        for (int i = 0; i < NUM_VERTEX_ATTRIBS; i++) {
            glEnableVertexAttribArray(i);
            Vertex2D::attribs[i].setup();
        }

        Glyph temp(posRect, uvRect, texture, NTA_DEFAULT_DEPTH, color, 0);
        Vertex2D vertices[6] = {
            temp.topLeft, temp.topRight, temp.botLeft,
            temp.topRight, temp.botLeft, temp.botRight
        };
        glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vertex2D), vertices, GL_STREAM_DRAW);
        glBindVertexArray(0);
    }
    void InstanceBatch::add_instance_data(const VertexAttrib* attribs, std::size_t num_attribs,
                                          void* data, GLsizeiptr size, GLuint divisor) {
        assert(num_attribs > 0);
        GLuint vbo = m_glyphs.back().gen_vbo();
        GLsizei new_count = size/attribs[0].stride;

        if (m_glyphs.back().count == 1) {
            m_glyphs.back().count = new_count;
        } else {
            /// \todo Push error to ErrorManager?
            assert(m_glyphs.back().count == new_count);
        }

        glBindVertexArray(m_glyphs.back().vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        for (int i = 0; i < num_attribs; i++) {
            glEnableVertexAttribArray(attribs[i].index);
            attribs[i].setup();
            glVertexAttribDivisor(attribs[i].index, divisor);
        }
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
        glBindVertexArray(0);
    }
    void InstanceBatch::render() const {
        for (const auto& glyph : m_glyphs) {
            glBindVertexArray(glyph.vao);
            glBindTexture(GL_TEXTURE_2D, glyph.tex);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, glyph.count);
            glBindVertexArray(0);
        }
    }
}
