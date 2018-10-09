#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string, RawTexture>                   ResourceManager::m_textureMap;
    std::map<std::pair<std::string,int>,SpriteFont*>    ResourceManager::m_fontMap;
    Result<RawTexture> ResourceManager::getTexture(crstring imagePath, 
                                                   crvec2 dimensions) {
        if (m_textureMap.find(imagePath) == m_textureMap.end()) {
            Result<RawTexture> res = ImageLoader::readImage(imagePath, dimensions);
            if (res.is_ok()) m_textureMap[imagePath] = res.get_data();
            return res;
        }
        return Result<RawTexture>::new_ok(m_textureMap[imagePath]);
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
            Logger::writeToLog("Deleting RawTexture: " + imagePath + "...");
            //glDeleteTextures(1, &m_textureMap[imagePath].id);
            delete[] m_textureMap[imagePath].data;
            Logger::writeToLog("Deleted RawTexture");
            m_textureMap.erase(imagePath);
        }
    }
    void ResourceManager::deleteSpriteFont(crstring fontPath, int fontSize) {
        std::pair<std::string, int> key = std::make_pair(fontPath, fontSize);
        if (m_fontMap.find(key) != m_fontMap.end()) {
            Logger::writeToLog("Deleting SpriteFont: " + fontPath +
                               " (" + utils::to_string(fontSize) + ")...");
            delete m_fontMap[key];
            m_fontMap.erase(key);
            Logger::writeToLog("Deleted SpriteFont");
        }
    }
    void ResourceManager::destroy() {
        Logger::writeToLog("Destroying ResourceManager...");
        Logger::indent();
        while (!m_textureMap.empty()) {
            deleteTexture(m_textureMap.begin()->first);
        }
        while (!m_fontMap.empty()) {
            // This is a dumb line
            deleteSpriteFont(m_fontMap.begin()->first.first, m_fontMap.begin()->first.second);
        }
        m_textureMap.clear();
        m_fontMap.clear();
        Logger::unindent();
        Logger::writeToLog("Destroyed ResourceManager");
    }
}
