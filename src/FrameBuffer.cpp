#include "nta/FrameBuffer.h"
#include "nta/Logger.h"

namespace nta {
    void FrameBuffer::init() {
        if (m_fbo != 0) destroy();
        Logger::writeToLog("Initializing FrameBuffer...");
        glGenFramebuffers(1, &m_fbo);
        Logger::writeToLog("Initialized FrameBuffer");
    }
    void FrameBuffer::use() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }
    void FrameBuffer::set_texture(GLuint idx) const {
        if (idx >= m_texs.size()) return;
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + idx);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + idx);
    }
    void FrameBuffer::unuse() const {
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    GLuint FrameBuffer::add_texture(GLuint width, GLuint height, bool rgba) {
        Logger::writeToLog("Adding texture to FrameBuffer " + std::to_string(m_fbo) + "...");
        use();

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        auto format = rgba ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_texs.size(), 
                               GL_TEXTURE_2D, tex, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_texs.push_back(tex);
        unuse();
        Logger::writeToLog("Added texture " + std::to_string(tex) + " associated with color attachment " + std::to_string(m_texs.size()-1));
        return m_texs.size()-1;
    }
    GLuint FrameBuffer::get_tex(GLuint idx) const {
        return idx < m_texs.size() ? m_texs[idx] : 0;
    }
    GLuint FrameBuffer::get_width(GLuint idx) const {
        if (idx >= m_texs.size()) return 0;
        glBindTexture(GL_TEXTURE_2D, m_texs[idx]);
        GLint width;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glBindTexture(GL_TEXTURE_2D, 0);
        return width;
    }
    GLuint FrameBuffer::get_height(GLuint idx) const {
        if (idx >= m_texs.size()) return 0;
        glBindTexture(GL_TEXTURE_2D, m_texs[idx]);
        GLint height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glBindTexture(GL_TEXTURE_2D, 0);
        return height;
    }
    void FrameBuffer::destroy() {
        Logger::writeToLog("Destroying FrameBuffer...");
        if (m_fbo != 0) glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(m_texs.size(), &m_texs[0]);
        m_texs.clear();
        m_fbo = 0;
        Logger::writeToLog("Destroyed FrameBuffer");
    }
}
