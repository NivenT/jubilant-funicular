#include <SDL2/SDL_ttf.h>

#include <iostream>
#include "GLMConsoleOutput.h"

#include "SpriteFont.h"
#include "Logger.h"

namespace nta {
    SpriteFont::SpriteFont(crstring fontPath, unsigned int size) {
        static auto toPower2 = [](int i) {
            for (int n = 16; n >= 0; n--) {
                if (i > (1<<n)) {
                    return (1<<(n+1));
                }
            }
        };
        if (!TTF_WasInit()) {
            TTF_Init();
        }
        Logger::writeToLog("Loading ttf font: " + fontPath + "...");
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
        if (!font) {
            Logger::writeErrorToLog("Could not load font");
        }
        m_fontHeight = TTF_FontHeight(font);
        Logger::writeToLog("Loaded font");
        Logger::writeToLog("Creating sprite font (" + toString(size) + ")...");
        FontMap* seed = new FontMap;
        SDL_Surface* glyphSurface = nullptr;
        for (char c = FIRST_PRINTABLE_CHAR; c <= LAST_PRINTABLE_CHAR; c++) {
            glyphSurface = TTF_RenderGlyph_Blended(font, c, {255, 255, 255, 255});
            seed->addRect(c, glm::vec2(glyphSurface->w, glyphSurface->h));
            SDL_FreeSurface(glyphSurface);
        }
        static Search::SimulatedAnnealer annealer;
        seed = (FontMap*)annealer.getSolution(seed, 1); seed->position();
        glm::ivec2 dimensions = glm::ivec2(toPower2(seed->getBoundingDimensions().x), toPower2(seed->getBoundingDimensions().y));
        //create initial gray texture
        glGenTextures(1, &m_texId);
        glBindTexture(GL_TEXTURE_2D, m_texId);
        GLubyte* graySquare = new GLubyte[dimensions.x*dimensions.y*4];
        memset(graySquare, 0x50, dimensions.x*dimensions.y*4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, graySquare);
        //add glyphs to texture
        m_charGlyphs = new CharGlyph[NUM_PRINTABLE_CHARS];
        for (char c = FIRST_PRINTABLE_CHAR; c <= LAST_PRINTABLE_CHAR; c++) {
            CharGlyph& cg = m_charGlyphs[c - FIRST_PRINTABLE_CHAR];
            FontMap::CharRect cr = seed->m_rects[c - FIRST_PRINTABLE_CHAR];
            cg.size = cr.dimensions;
            cg.uvRect = glm::vec4(cr.topLeft/glm::vec2(dimensions), cr.dimensions/glm::vec2(dimensions));
            cg.uvRect.y *= -1;
            glyphSurface = TTF_RenderGlyph_Blended(font, c, {255, 255, 255, 255});
            glTexSubImage2D(GL_TEXTURE_2D, 0, cr.topLeft.x, -cr.topLeft.y, cr.dimensions.x, cr.dimensions.y, GL_RGBA,
                            GL_UNSIGNED_BYTE, glyphSurface->pixels);
            SDL_FreeSurface(glyphSurface);
        }
        glyphSurface = nullptr;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        TTF_CloseFont(font);
        Logger::writeToLog("Created sprite font");
        Logger::writeToLog("Generated font has dimensions: " + toString(dimensions.x) + " x " + toString(dimensions.y));
    }
    SpriteFont::~SpriteFont() {
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
    void SpriteFont::drawText(SpriteBatch& batch, crstring text, crvec2 topLeft, crvec2 scale, crvec4 color, float depth) const {
        glm::vec2 offset(0);
        for (char c : text) {
            if (c == '\n') {
                offset.y -= m_fontHeight*scale.y;
                offset.x = 0;
            } else {
                CharGlyph cg = m_charGlyphs[c-FIRST_PRINTABLE_CHAR];
                batch.addGlyph(glm::vec4(topLeft+offset, cg.size*scale), cg.uvRect, m_texId, depth, color);
                offset.x += cg.size.x*scale.x;
            }
        }
    }
    void SpriteFont::drawText(SpriteBatch& batch, crstring text, crvec4 posRect, crvec4 color, float depth) const {
        glm::vec2 scale = glm::vec2(posRect[2], posRect[3])/measure(text);
        drawText(batch, text, glm::vec2(posRect.x, posRect.y), scale, color, depth);
    }
    void SpriteFont::drawTexture(SpriteBatch& batch) const {
        batch.addGlyph(glm::vec4(-100,100,200,200), glm::vec4(0,0,1,1), m_texId, 1);
    }
}
