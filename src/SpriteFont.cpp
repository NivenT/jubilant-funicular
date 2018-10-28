#include "nta/SpriteFont.h"
#include "nta/Logger.h"

namespace nta {
    void SpriteFont::init(TTF_Font* font) {
        /*
        static auto toPower2 = [](int i) {
            i--;
            i |= i >> 1;
            i |= i >> 2;
            i |= i >> 4;
            i |= i >> 8;
            i |= i >> 16;
            return ++i;
        };
        */
        m_fontHeight = TTF_FontHeight(font);
        Logger::writeToLog("Creating SpriteFont...");
        FontMap* seed = new FontMap; /// \todo Rename now that we're no longer using simulated annealing
        SDL_Surface* glyphSurface = nullptr;
        for (char c = FIRST_PRINTABLE_CHAR; c <= LAST_PRINTABLE_CHAR; c++) {
            glyphSurface = TTF_RenderGlyph_Blended(font, c, {255, 255, 255, 255});
            seed->addRect(c, glm::vec2(glyphSurface->w, glyphSurface->h));
            SDL_FreeSurface(glyphSurface);
        }
        seed->position();
        /* Why did I want these to be powers of 2 before?
        glm::ivec2 dimensions = glm::ivec2(toPower2(seed->getBoundingDimensions().x),
                                           toPower2(seed->getBoundingDimensions().y));
        */
        glm::ivec2 dimensions(ceil(seed->getBoundingDimensions().x),
                              ceil(seed->getBoundingDimensions().y));
        // create initial gray texture
        glGenTextures(1, &m_texId);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        GLubyte* graySquare = new GLubyte[dimensions.x*dimensions.y*4];
        memset(graySquare, 0x50, dimensions.x*dimensions.y*4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, graySquare);
        delete[] graySquare;
        // add glyphs to texture
        m_charGlyphs = new CharGlyph[NUM_PRINTABLE_CHARS];
        for (char c = FIRST_PRINTABLE_CHAR; c <= LAST_PRINTABLE_CHAR; c++) {
            CharGlyph& cg = m_charGlyphs[c - FIRST_PRINTABLE_CHAR];
            FontMap::CharRect cr = seed->m_rects[c - FIRST_PRINTABLE_CHAR];
            cg.size = cr.dimensions;
            cg.uvRect = glm::vec4(cr.topLeft/glm::vec2(dimensions),
                                  cr.dimensions/glm::vec2(dimensions));
            cg.uvRect.y *= -1;
            glyphSurface = TTF_RenderGlyph_Blended(font, c, {255, 255, 255, 255});
            glTexSubImage2D(GL_TEXTURE_2D, 0, cr.topLeft.x, -cr.topLeft.y, cr.dimensions.x,
                            cr.dimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, glyphSurface->pixels);
            SDL_FreeSurface(glyphSurface);
        }
        glyphSurface = nullptr;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        Logger::writeToLog("Created SpriteFont");
        Logger::writeToLog("Generated font has dimensions: " + utils::to_string(dimensions.x) + " x " +
                           utils::to_string(dimensions.y));
    }
    void SpriteFont::destroy() {
        if (m_texId != 0) {
            glDeleteTextures(1, &m_texId);
        }
        m_texId = 0;
        if (m_charGlyphs) {
            delete[] m_charGlyphs;
        }
        m_charGlyphs = nullptr;
    }
    glm::vec2 SpriteFont::measure(crstring text) const {
        if (text == "") {
            return glm::vec2(0);
        }
        glm::vec2 dim(0, m_fontHeight);
        float xOffset = 0;
        for (char c : text) {
            if (c == '\n') {
                dim.y += m_fontHeight;
                xOffset = 0;
            } else {
                xOffset += m_charGlyphs[c-FIRST_PRINTABLE_CHAR].size.x;
                if (xOffset > dim.x) {
                    dim.x = xOffset;
                }
            }
        }
        return dim;
    }
    void SpriteFont::drawText(SpriteBatch& batch, crstring text, crvec2 topLeft, crvec2 scale,
                              crvec4 color, float depth) const {
        glm::vec2 offset(0);
        for (char c : text) {
            if (c == '\n') {
                offset.y -= m_fontHeight*scale.y;
                offset.x = 0;
            } else {
                CharGlyph cg = m_charGlyphs[c-FIRST_PRINTABLE_CHAR];
                batch.addGlyph(glm::vec4(topLeft+offset, cg.size*scale), cg.uvRect, m_texId,
                               depth, color);
                offset.x += cg.size.x*scale.x;
            }
        }
    }
    void SpriteFont::drawText(SpriteBatch& batch, crstring text, crvec4 posRect, crvec4 color,
                              float depth) const {
        glm::vec2 scale = glm::vec2(posRect[2], posRect[3])/measure(text);
        drawText(batch, text, glm::vec2(posRect.x, posRect.y), scale, color, depth);
    }
    void SpriteFont::drawText(SpriteBatch& batch, crvec2 corner1, crvec2 corner2, crstring text, 
                              crvec4 color, float depth) const {
        glm::vec4 posRect(glm::min(corner1.x, corner2.x), glm::max(corner1.y, corner2.y), 
                          glm::abs(corner1-corner2));
        drawText(batch, text, posRect, color, depth);
    }
    void SpriteFont::drawTexture(SpriteBatch& batch, crvec4 posRect) const {
        batch.addGlyph(posRect, glm::vec4(0,0,1,1), m_texId, 1);
    }
}
