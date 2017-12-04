#include "ResourceManager.h"
#include "Logger.h"

namespace nta {
    std::map<std::string,GLTexture>                     ResourceManager::m_textureMap;
    std::map<std::pair<std::string,int>,SpriteFont*>    ResourceManager::m_fontMap;
    GLTexture& ResourceManager::getTexture(crstring imagePath, GLint minFilt, GLint magFilt, crvec2 dimensions) {
        std::string folder = "";
        if (imagePath.find("resources/images/") != 0)
            folder += "resources/images/";
        if (m_textureMap.find(folder+imagePath) == m_textureMap.end()) {
            m_textureMap[folder+imagePath] = ImageLoader::readImage(folder+imagePath, minFilt,
                                                                    magFilt, dimensions);
        }
        return m_textureMap[folder+imagePath];
    }
    SpriteFont* ResourceManager::getSpriteFont(crstring fontPath, int fontSize) {
        std::string folder = "";
        if (fontPath.find("resources/fonts/") != 0)
            folder = "resources/fonts/";
        std::pair<std::string, int> key = std::make_pair(folder+fontPath, fontSize);
        if (m_fontMap.find(key) == m_fontMap.end()) {
            m_fontMap[key] = new SpriteFont(folder+fontPath, fontSize);
        }
        return m_fontMap[key];
    }
    void ResourceManager::deleteTexture(crstring imagePath) {
        std::string folder = "";
        if (imagePath.find("resources/images/") != 0) {
            folder += "resources/images/";
        }
        if (m_textureMap.find(folder+imagePath) != m_textureMap.end()) {
            glDeleteTextures(1, &m_textureMap[folder+imagePath].id);
            m_textureMap.erase(folder+imagePath);
        }
    }
    void ResourceManager::deleteSpriteFont(crstring fontPath, int fontSize) {
        std::string folder = "";
        if (fontPath.find("resources/fonts/") != 0) {
            folder = "resources/fonts/";
        }
        std::pair<std::string, int> key = std::make_pair(folder+fontPath, fontSize);
        if (m_fontMap.find(key) != m_fontMap.end()) {
            delete m_fontMap[key];
            m_fontMap.erase(key);
        }
    }
    void ResourceManager::destroy() {
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
    }
}
