#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string,GLTexture>                     ResourceManager::m_textureMap;
    std::unordered_map<GLTexture, std::string>          ResourceManager::m_textureFiles;
    std::map<std::pair<std::string,int>,SpriteFont*>    ResourceManager::m_fontMap;
    Result<GLTexture> ResourceManager::getTexture(crstring imagePath, GLint minFilt, 
                                                   GLint magFilt, crvec2 dimensions) {
        if (m_textureMap.find(imagePath) == m_textureMap.end()) {
            Result<GLTexture> res = ImageLoader::readImage(imagePath, minFilt,
                                                           magFilt, dimensions);
            if (res.is_ok()) m_textureMap[imagePath] = res.get_data();
            m_textureFiles[res.get_data()] = imagePath;
            return res;
        }
        return Result<GLTexture>::new_ok(m_textureMap[imagePath]);
    }
    Result<std::string> ResourceManager::getFile(GLTexture tex) {
        if (m_textureFiles.find(tex) == m_textureFiles.end()) {
            auto err = Logger::writeErrorToLog("tex not found in ResourceManager::m_textureFiles", MISSING_RESOURCE);
            return Result<std::string>::new_err(err);
        }
        return Result<std::string>::new_ok(m_textureFiles[tex]);
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
                               " (" + utils::to_string(fontSize) + ")...");
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
                               " (" + utils::to_string(pair.first.second) + ")...");
            delete pair.second;
            Logger::writeToLog("Deleted sprite font");
        }
        m_textureMap.clear();
        m_textureFiles.clear();
        m_fontMap.clear();
        Logger::unindent();
        Logger::writeToLog("Destroyed ResourceManager");
    }
}
