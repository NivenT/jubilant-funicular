#ifndef NTA_INSTANCEBATCH_H_INCLUDED
#define NTA_INSTANCEBATCH_H_INCLUDED

#include "nta/SpriteBatch.h"

namespace nta {
    /// Represents a Glyph to be drawn multiple times with slight variations
    struct InstancedGlyph {
        InstancedGlyph(GLuint texture) : count(1), tex(texture) {
            glGenVertexArrays(1, &vao);
        }
        GLuint gen_vbo() {
            vbos.push_back(0);
            glGenBuffers(1, &vbos.back());
            return vbos.back();
        }

        /// The number of instances
        GLsizei count;
        /// The texture
        GLuint tex;
        GLuint vao;
        std::vector<GLuint> vbos;
    };
    /// \todo Compare speed of this and SpriteBatch when there's no instancing being done
    class InstanceBatch {
    private:
        std::vector<InstancedGlyph> m_glyphs;
    public:
        InstanceBatch() {}
        ~InstanceBatch() { begin(); }
        /// Clears out data from the previous frame
        void begin();
        /// Doesn't actually do anything. This is purely cosmetic
        void end();
        /// \todo (?) add depth
        void add_glyph(crvec4 posRect, GLuint texture = 0, 
                       crvec4 uvRect = glm::vec4(0,0,1,1), 
                       crvec4 color = glm::vec4(1));
        /// Adds data that varies between instances.
        ///
        /// attribs describes the glsl "in" variables to change
        ///
        /// OpenGL fetches new data after divisor instances are rendered
        void add_instance_data(const VertexAttrib* attribs, std::size_t num_attribs, 
                               void* data, GLsizeiptr size, GLuint divisor = 1);
        void render() const;
    };
}

#endif // NTA_INSTANCEBATCH_H_INCLUDED
