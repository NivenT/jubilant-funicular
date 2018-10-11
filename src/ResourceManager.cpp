#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<std::string, RawTexture>                   ResourceManager::m_textureMap;
    std::map<std::pair<std::string, int>, TTF_Font*>    ResourceManager::m_fontMap;
    Result<RawTexture> ResourceManager::getTexture(crstring imagePath, 
                                                   crvec2 dimensions) {
        if (m_textureMap.find(imagePath) == m_textureMap.end()) {
            Result<RawTexture> res = ImageLoader::readImage(imagePath, dimensions);
            if (res.is_ok()) m_textureMap[imagePath] = res.get_data();
            return res;
        }
        return Result<RawTexture>::new_ok(m_textureMap[imagePath]);
    }
    Result<TTF_Font*> ResourceManager::getFont(crstring fontPath, int fontSize) {
        std::pair<std::string, int> key = std::make_pair(fontPath, fontSize);
        if (m_fontMap.find(key) == m_fontMap.end()) {
            Logger::writeToLog("Loading ttf font: " + fontPath + "...");
            auto font = TTF_OpenFont(fontPath.c_str(), fontSize);
            if (!font) {
                auto err_str = "SDL_TTF failed to load font \"" + fontPath +
                               "\" with error: " + TTF_GetError();
                auto err = Logger::writeErrorToLog(err_str, SDL_FAILURE);
                return Result<TTF_Font*>::new_err(err);
            }
            Logger::writeToLog("Loaded font");
            m_fontMap[key] = font;
        }
        return Result<TTF_Font*>::new_ok(m_fontMap[key]);
    }
    void ResourceManager::destroy() {
        Logger::writeToLog("Destroying ResourceManager...");
        Logger::indent();
        for (auto& pair : m_textureMap) {
            Logger::writeToLog("Deleting RawTexture: " + pair.first + "...");
            delete[] pair.second.data;
            Logger::writeToLog("Deleted RawTexture");
        }
        for (auto& pair : m_fontMap) {
            Logger::writeToLog("Deleting TTF_Font*: " + pair.first.first + "...");
            TTF_CloseFont(pair.second);
            Logger::writeToLog("Deleted TTF_Font*");
        }
        m_textureMap.clear();
        m_fontMap.clear();
        Logger::unindent();
        Logger::writeToLog("Destroyed ResourceManager");
    }
}
