#ifndef NTA_ANIMATION2D_H_INCLUDED
#define NTA_ANIMATION2D_H_INCLUDED

#include "nta/ContextData.h"

namespace nta {
    /// Multiple sprites (each the same size) in one texture
    struct SpriteSheet {
        SpriteSheet() {
            tex.id = 0;
            dims.x = dims.y = 0;
        }
        SpriteSheet(const GLTexture& tex, crivec2 dims) : tex(tex), dims(dims) {
        }
        SpriteSheet(ContextData& context, crstring file_path, crivec2 dims);
        SpriteSheet(ContextData& context, crstring file_path, int num_cols) : SpriteSheet(context, file_path, glm::ivec2(1, num_cols)) {
        }
        /// Gets UV-coordinates of specific sprite in the sheet
        ///
        /// The topleft sprite is index 0 and indices increase moving to the right
        glm::vec4 get_uv(std::size_t index) const {
            std::size_t r = index/num_cols;
            std::size_t c = index%num_cols;
            float w = 1.f/num_cols;
            float h = 1.f/num_rows;
            return glm::vec4(c*w, r*h, w, h);
        }
        /// Like get_uv except it flips the sprite horizontally
        glm::vec4 get_flipped_uv(std::size_t index) const {
            auto coords = get_uv(index);
            coords.x += coords[2];
            coords[2] *= -1;
            return coords;
        }
        /// Returns the dimensions of one sprite in this sheet
        glm::vec2 get_frame_dims() const {
            return glm::vec2(tex.width/num_cols, tex.height/num_rows);
        }
        /// The texture holding all the sprites
        GLTexture tex;
        /// The dimensions of the sheat (rows x cols)
        union {
            glm::ivec2 dims;
            struct {
                int num_rows;
                int num_cols;
            };
        };
    };
    /// A 2D Animation made from a single SpriteSheet
    class Animation2D {
    private:
        /// The sheet holding the sprites used by the animation
        SpriteSheet m_sheet;
        /// how far along in the animation we are
        float m_time;
        /// how quickly the animation advances when step is called
        float m_speed;
        /// A single animation spans m_length continuous sprite indices begging with m_start_index
        std::size_t m_start_index, m_length;
    public:
        Animation2D() {}
        Animation2D(const SpriteSheet& sheet, std::size_t start = 0, 
                    std::size_t length = 1, float speed = 1);
        Animation2D(ContextData& context, crstring file_path, crivec2 dims, 
                    std::size_t start = 0, std::size_t length = 1, float speed = 1);
        Animation2D(ContextData& context, crstring file_path, int num_cols, 
                    std::size_t start = 0, std::size_t length = 1, float speed = 1);
        glm::vec4 get_uv() const;
        glm::vec4 get_flipped_uv() const;
        glm::vec2 get_frame_dims() const;
        std::size_t get_index() const;
        std::size_t get_start() const;
        std::size_t get_length() const;
        float get_speed() const;
        GLuint get_tex_id() const;
        float get_time() const;
        void switch_animation(std::size_t start, std::size_t length, float speed = 1);
        // ugh I have a setter and a getter (maybe I should just make m_speed public...)
        void set_speed(float speed);
        void step(float dt);
    };
}

#endif // NTA_ANIMATION2D_H_INCLUDED
