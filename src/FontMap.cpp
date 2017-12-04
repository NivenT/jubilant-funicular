#include <algorithm>

#include "SpriteFont.h"
#include "Random.h"

namespace nta {
    FontMap::FontMap() {
        m_rects = new CharRect[NUM_PRINTABLE_CHARS];
        for (int i = 0; i < NUM_PRINTABLE_CHARS; i++) {
            m_rects[i].addedToMap = false;
        }
    }
    FontMap::~FontMap() {
        delete[] m_rects;
    }
    glm::vec2 FontMap::getBoundingDimensions() const {
        float minX = 999, maxX = -999, minY = 999, maxY = -999;
        std::for_each(m_rects, m_rects+NUM_PRINTABLE_CHARS, [&](const CharRect& r) {
                minX = std::min(r.topLeft.x,                minX);
                maxX = std::max(r.topLeft.x+r.dimensions.x, maxX);
                minY = std::min(r.topLeft.y-r.dimensions.y, minY);
                maxY = std::max(r.topLeft.y,                maxY);
            });
        return glm::vec2(maxX-minX, maxY-minY);
    }
    bool FontMap::isOverlapping(const CharRect& rect) const {
        return std::any_of(m_rects, m_rects+NUM_PRINTABLE_CHARS, [&](const CharRect& r) {
                if (r.addedToMap) {
                    glm::vec4 rectC(rect.topLeft.x+rect.dimensions.x/2, rect.topLeft.y-rect.dimensions.y/2, rect.dimensions.x/2.f, rect.dimensions.y/2.f);
                    glm::vec4 rC(r.topLeft.x+r.dimensions.x/2, r.topLeft.y-r.dimensions.y/2, r.dimensions.x/2.f, r.dimensions.y/2.f);
                    glm::vec4 displacement = rectC-rC;
                    if (fabs(displacement.x) < rectC[2]+rC[2] && fabs(displacement.y) < rectC[3]+rC[3]) {
                        return true;
                    }
                }
                return false;
            });
    }
    void FontMap::addRect(char c, crvec2 dimensions) {
        CharRect newRect;
        if (!std::any_of(m_rects, m_rects+NUM_PRINTABLE_CHARS, [](const CharRect& cr){return cr.addedToMap;})) {
            newRect = {glm::vec2(0), dimensions, true};
        } else do {
                CharRect existingRect;
                do {
                    existingRect = m_rects[Random::randInt(NUM_PRINTABLE_CHARS)];
                } while(!existingRect.addedToMap);
                float width = dimensions.x;
                float height = dimensions.y;
                int relPos = Random::randInt(4);
                switch(relPos) {
                    //top
                case 0: newRect = {glm::vec2(existingRect.topLeft.x, existingRect.topLeft.y+height), dimensions, true}; break;
                    //bot
                case 1: newRect = {glm::vec2(existingRect.topLeft.x, existingRect.topLeft.y-existingRect.dimensions.y), dimensions, true}; break;
                    //left
                case 2: newRect = {glm::vec2(existingRect.topLeft.x-width, existingRect.topLeft.y), dimensions, true}; break;
                    //right
                case 3: newRect = {glm::vec2(existingRect.topLeft.x+existingRect.dimensions.x, existingRect.topLeft.y), dimensions, true}; break;
                }
            } while (isOverlapping(newRect));
        m_rects[c-FIRST_PRINTABLE_CHAR] = newRect;
    }
    void FontMap::position() {
        glm::vec2 topLeft = m_rects[0].topLeft;
        std::for_each(m_rects, m_rects+NUM_PRINTABLE_CHARS, [&](const CharRect& rect) {
                if (rect.topLeft.x < topLeft.x) {
                    topLeft.x = rect.topLeft.x;
                }
                if (rect.topLeft.y > topLeft.y) {
                    topLeft.y = rect.topLeft.y;
                }
            });
        //translate so topLeft is (0,0)
        std::for_each(m_rects, m_rects+NUM_PRINTABLE_CHARS, [&](CharRect& rect) {
                rect.topLeft -= topLeft;
            });
    }
}
