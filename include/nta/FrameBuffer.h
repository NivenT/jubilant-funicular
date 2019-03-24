#ifndef NTA_FRAMEBUFFER_H_INCLUDED
#define NTA_FRAMEBUFFER_H_INCLUDED

#include <vector>

#include <GL/glew.h>

namespace nta {
    /// A wrapper around an OpenGL Framebuffer object
    class FrameBuffer {
    private:
        /// The id of this frame buffer
        GLuint m_fbo;
        /// The textures to draw to
        std::vector<GLuint> m_texs;
        /// The render buffer objects (currently unused)
        std::vector<GLuint> m_rbos;
    public:
        FrameBuffer() : m_fbo(0) {}
        ~FrameBuffer() { destroy(); }

        /// Initializes a new FrameBuffer (deleting the old one if it existed)
        void init();
        /// Binds this FrameBuffer
        void use() const;
        /// Sets which texture to draw to
        void set_texture(GLuint idx) const;
        /// Binds the default FrameBuffer (i.e. the screen)
        void unuse() const;

        /// Returns index of added texture
        GLuint add_texture(GLuint width, GLuint height, bool rgba = true);
        /// Returns id of texture associated with GL_COLOR_ATTACHMENTidx
        GLuint get_tex(GLuint idx) const;
        /// Returns width of texture associated with GL_COLOR_ATTACHMENTidx
        GLuint get_width(GLuint idx) const;
        /// Returns height of texture associated with GL_COLOR_ATTACHMENTidx
        GLuint get_height(GLuint idx) const;
        GLuint num_texs() const { return m_texs.size(); }

        /// Destroys this FrameBuffer
        void destroy();
    };
}

#endif // NTA_FRAMEBUFFER_H_INCLUDED
