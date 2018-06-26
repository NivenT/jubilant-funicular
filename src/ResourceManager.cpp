#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string,GLTexture>                     ResourceManager::m_textureMap;
    std::map<std::pair<std::string,int>,SpriteFont*>    ResourceManager::m_fontMap;
    Result<GLTexture> ResourceManager::getTexture(crstring imagePath, GLint minFilt, 
                                                   GLint magFilt, crvec2 dimensions) {
        if (m_textureMap.find(imagePath) == m_textureMap.end()) {
            Result<GLTexture> res = ImageLoader::readImage(imagePath, minFilt,
                                                           magFilt, dimensions);
            if (res.is_ok()) m_textureMap[imagePath] = res.get_data();
            return res;
        }
        return Result<GLTexture>::new_ok(m_textureMap[imagePath]);
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
            Logger::writeToLog("Deleting texture: " + imagePath + "...");
            glDeleteTextures(1, &m_textureMap[imagePath].id);
            Logger::writeToLog("Deleted texture");
            m_textureMap.erase(imagePath);
        }
    }
    void ResourceManager::deleteSpriteFont(crstring fontPath, int fontSize) {
        std::pair<std::string, int> key = std::make_pair(fontPath, fontSize);
        if (m_fontMap.find(key) != m_fontMap.end()) {
            Logger::writeToLog("Deleting sprite font: " + fontPath +
                               " (" + to_string(fontSize) + ")...");
            delete m_fontMap[key];
            m_fontMap.erase(key);
            Logger::writeToLog("Deleted sprite font");
        }
    }
    void ResourceManager::destroy() {
        Logger::writeToLog("Destroying ResourceManager...");
        Logger::indent();
        for (auto pair : m_textureMap) {
            Logger::writeToLog("Deleting texture: " + pair.first + "...");
            glDeleteTextures(1, &pair.second.id);
            Logger::writeToLog("Deleted texture");
        }
        for (auto pair : m_fontMap) {
            Logger::writeToLog("Deleting sprite font: " + pair.first.first +
                               " (" + to_string(pair.first.second) + ")...");
            delete pair.second;
            Logger::writeToLog("Deleted sprite font");
        }
        m_textureMap.clear();
        m_fontMap.clear();
        Logger::unindent();
        Logger::writeToLog("Destroyed ResourceManager");
    }
}
