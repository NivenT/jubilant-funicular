#ifndef NTA_SPRITEFONT_H_INCLUDED
#define NTA_SPRITEFONT_H_INCLUDED

#include <map>

#include <SDL2/SDL_ttf.h>

#include "nta/SpriteBatch.h"

#define FIRST_PRINTABLE_CHAR ((char)32)  //space
#define LAST_PRINTABLE_CHAR  ((char)126) //~
#define NUM_PRINTABLE_CHARS  (LAST_PRINTABLE_CHAR-FIRST_PRINTABLE_CHAR+1)

namespace nta {
    class SpriteFont;
    class ContextData;
    /// represents a single char in the texture
    struct CharGlyph {
        /// the rectangle containing this glyph in the texture
        glm::vec4 uvRect;
        /// the size of the rendered glyph
        glm::vec2 size;
    };
    /// represents the organization of a texture containing the characters
    class FontMap {
    private:
        /// a rectangle representing the location of a char in the FontMap
        struct CharRect {
            CharRect(){}
            CharRect(glm::vec2 tl, glm::vec2 d, bool a) : topLeft(tl), dimensions(d), addedToMap(a) {
            }
            glm::vec2 topLeft;
            glm::vec2 dimensions;
            bool addedToMap;
        };
        /// returns whether or not rect is overlapping with any existing CharRect
        bool isOverlapping(const CharRect& rect) const;
        /// the rectangles making up the FontMap
        CharRect* m_rects = nullptr;
    public:
        /// constructor and destructor
        FontMap();
        ~FontMap();
        /// returns the dimensions of the rectangle that contains the FontMap
        glm::vec2 getBoundingDimensions() const;
        /// adds a rectangle and associates it with c (replacing any preexisting rectangle)
        void addRect(char c, crvec2 dimensions);
        /// positions map so that the topleft is at (0,0)
        void position();

        friend SpriteFont;
    };
    /// Loads in a .ttf file, creates a font texture from it, and is then used to render text
    class SpriteFont {
    private:
        /// Initializes the SpriteFont
        ///
        /// Creates a texture from the font
        void init(TTF_Font* font);
        /// a collection of glyphs for each char
        CharGlyph* m_charGlyphs = nullptr;
        /// the idea of the generated texture
        GLuint m_texId = 0;
        /// the height of the font
        int m_fontHeight = 0;
    public:
        SpriteFont() {}
        /// destructor
        ~SpriteFont() { destroy(); }
        /// returns the dimensions of the rectangle containing the text
        glm::vec2 measure(crstring text) const;
        /// renders text with specified location, color, scale, etc.
        void drawText(SpriteBatch& batch, crstring text, crvec2 topLeft, crvec2 scale,
                      crvec4 color = glm::vec4(1), float depth = NTA_DEFAULT_DEPTH) const;
        void drawText(SpriteBatch& batch, crstring text, crvec4 posRect, crvec4 color = glm::vec4(1),
                      float depth = NTA_DEFAULT_DEPTH) const;
        void drawText(SpriteBatch& batch, crvec2 corner1, crvec2 corner2, crstring text,
                      crvec4 color = glm::vec4(1), float depth = NTA_DEFAULT_DEPTH) const;
        /// renders texture
        void drawTexture(SpriteBatch& batch, crvec4 posRect = glm::vec4(-100, 100, 200, 200)) const;
        /// Destroys this SpriteFont
        void destroy();
        friend ContextData;
    };
}

#endif // NTA_SPRITEFONT_H_INCLUDED
