#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string,GLTexture>                     ResourceManager::m_textureMap;
    std::map<std::pair<std::string,int>,SpriteFont*>    ResourceManager::m_fontMap;
    GLTexture& ResourceManager::getTexture(crstring imagePath, GLint minFilt, GLint magFilt, crvec2 dimensions) {
        if (m_textureMap.find(imagePath) == m_textureMap.end()) {
            m_textureMap[imagePath] = ImageLoader::readImage(imagePath, minFilt,
                                                             magFilt, dimensions);
        }
        return m_textureMap[imagePath];
    }
    SpriteFont* ResourceManager::getSpriteFont(crstring fontPath, int fontSize) {
        std::pair<std::string, int> key = std::make_pair(fontPath, fontSize);
        if (m_fontMap.find(key) == m_fontMap.end()) {
            m_fontMap[key] = new SpriteFont(fontPath, fontSize);
        }
        return m_fontMap[key];
    }
    void ResourceManager::deleteTexture(crstring imagePath) {
        if (m_textureMap.find(imagePath) != m_textureMap.end()) {
            glDeleteTextures(1, &m_textureMap[imagePath].id);
            m_textureMap.erase(imagePath);
        }
    }
    void ResourceManager::deleteSpriteFont(crstring fontPath, int fontSize) {
        std::pair<std::string, int> key = std::make_pair(fontPath, fontSize);
        if (m_fontMap.find(key) != m_fontMap.end()) {
            delete m_fontMap[key];
            m_fontMap.erase(key);
        }
    }
    void ResourceManager::destroy() {
        Logger::writeToLog("Destroying ResourceManager...");
        for (auto pair : m_textureMap) {
            Logger::writeToLog("Deleting texture: " + pair.first + "...");
            glDeleteTextures(1, &pair.second.id);
            Logger::writeToLog("Deleted texture");
        }
        for (auto pair : m_fontMap) {
            Logger::writeToLog("Deleting sprite font: " + pair.first.first +
                               " (" +to_string(pair.first.second) + ")...");
            delete pair.second;
            Logger::writeToLog("Deleted sprite font");
        }
        m_textureMap.clear();
        m_fontMap.clear();
        Logger::writeToLog("Destroyed ResourceManager");
    }
}
