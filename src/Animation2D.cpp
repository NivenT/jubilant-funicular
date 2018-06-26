#include "nta/Animation2D.h"
#include "nta/ResourceManager.h"
#include "nta/Logger.h"

#define INVALID_LEN_MSG "Animation2D Error: Tried creating animation with 0 length"
#define INVALID_IDX_MSG "Animation2D Error: Tried creating animation start with nonexistent index"

namespace nta {
    SpriteSheet::SpriteSheet(crstring file_path, crivec2 dims) : dims(dims) {
        tex = ResourceManager::getTexture(file_path);
    }

    Animation2D::Animation2D(const SpriteSheet& sheet, std::size_t start, std::size_t length) :
        m_sheet(sheet), m_time(0), m_start_index(start), m_length(length) {
        if (m_length == 0) {
            Logger::writeErrorToLog(INVALID_LEN_MSG, INVALID_VALUE);
        }
        if (m_start_index >= m_sheet.num_rows*m_sheet.num_cols) {
            Logger::writeErrorToLog(INVALID_IDX_MSG, INVALID_VALUE);
        }
    }
    Animation2D::Animation2D(crstring file_path, crivec2 dims, std::size_t start, std::size_t length) :
        Animation2D(SpriteSheet(file_path, dims), start, length) {
    }
    Animation2D::Animation2D(crstring file_path, int num_cols, std::size_t start, std::size_t length) :
        Animation2D(SpriteSheet(file_path, num_cols), start, length) {
    }
    glm::vec4 Animation2D::get_uv() const {
        return m_sheet.get_uv(get_index());
    }
    glm::vec4 Animation2D::get_flipped_uv() const {
        return m_sheet.get_flipped_uv(get_index());
    }
    GLuint Animation2D::get_tex_id() const {
        return m_sheet.tex.id;
    }
    std::size_t Animation2D::get_index() const {
        return m_start_index + ((std::size_t)m_time)%m_length;
    }
    std::size_t Animation2D::get_start() const {
        return m_start_index;
    }
    std::size_t Animation2D::get_length() const {
        return m_length;
    }
    float Animation2D::get_time() const {
        return m_time;
    }
    void Animation2D::switch_animation(std::size_t start, std::size_t length) {
        m_start_index = start;
        m_length = length;
        m_time = 0;

        if (m_length == 0) {
            Logger::writeErrorToLog(INVALID_LEN_MSG, INVALID_VALUE);
        }
        if (m_start_index >= m_sheet.num_rows*m_sheet.num_cols) {
            Logger::writeErrorToLog(INVALID_IDX_MSG, INVALID_VALUE);
        }
    }
    void Animation2D::step(float dt) {
        m_time += dt;
    }
}