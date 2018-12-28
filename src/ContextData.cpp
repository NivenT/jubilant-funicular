#include "nta/ContextData.h"
#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    GLSLProgram* ContextData::getGLSLProgram(const utils::Path& progPath) {
        /// \todo call resolve (?)
        std::string full_path = (m_glsl_folder + progPath).to_string();
        if (m_glslMap.find(full_path) == m_glslMap.end()) {
            m_glslMap[full_path].compileShaders(full_path);
        }
        return &m_glslMap[full_path];
    }
    GLSLProgram* ContextData::getGLSLProgram(crstring name, const utils::Path& vert, const utils::Path& frag) {
        std::string full_vert_path = (m_glsl_folder + vert).to_string(),
                    full_frag_path = (m_glsl_folder + frag).to_string();
        if (m_glslMap.find(name) == m_glslMap.end()) {
            m_glslMap[name].compileShaders(full_vert_path, full_frag_path);
        }
        return &m_glslMap[name];
    }
    Result<GLTexture> ContextData::getTexture(const utils::Path& path, crvec2 dimensions) {
        utils::Path full_path = m_texture_folder + path;
        if (m_textureMap.find(full_path) == m_textureMap.end()) {
            std::string path_str = full_path.to_string();
            auto raw = ResourceManager::getTexture(path_str, dimensions);
            // I really like this bit
            return raw.map<GLTexture>([&](const RawTexture& raw) {
                Logger::writeToLog("RawTexture \"" + path_str + "\" doesn't have a GLTexture in this context.");
                return m_textureMap[full_path] = GLTexture(raw);
            });
        }
        return Result<GLTexture>::new_ok(m_textureMap[path]);
    }
    Result<utils::Path> ContextData::getTextureFile(GLTexture tex) {
        if (m_textureFiles.find(tex) == m_textureFiles.end()) {
            auto err = Logger::writeErrorToLog("tex not found in ContextData::m_textureFiles", MISSING_RESOURCE);
            return Result<utils::Path>::new_err(err);
        }
        return Result<utils::Path>::new_ok(m_textureFiles[tex]);
    }
    Result<SpriteFont*> ContextData::getSpriteFont(const utils::Path& fontPath, int fontSize) {
        std::pair<utils::Path, int> key = std::make_pair(m_font_folder + fontPath, fontSize);
        if (m_fontMap.find(key) == m_fontMap.end()) {
            std::string path_str = key.first.to_string();
            auto ttf_font = ResourceManager::getFont(path_str, fontSize);
            if (ttf_font.is_err()) return ttf_font.convert_error<SpriteFont*>();
            Logger::writeToLog("TTF_Font \"" + path_str + "\" (size: " + utils::to_string(fontSize) + ") doesn't have a SpriteFont in this context.");
            m_fontMap[key].init(ttf_font.unwrap());
        }
        return Result<SpriteFont*>::new_ok(&m_fontMap[key]);
    }
    void ContextData::destroy() {
        Logger::writeToLog("Destroying ContextData...");
        Logger::indent();
        for (auto& pair : m_glslMap) {
            Logger::writeToLog("Deleting GLSLProgram \"" + pair.first + "\"...");
            pair.second.destroy();
            Logger::writeToLog("Deleted Program");
        }
        for (auto& pair : m_textureMap) {
            Logger::writeToLog("Deleting GLTexture \"" + pair.first.to_string() + "\"...");
            pair.second.destroy();
            Logger::writeToLog("Deleted GLTexture");
        }
        for (auto& pair : m_fontMap) {
            Logger::writeToLog("Deleting SpriteFont \"" + pair.first.first.to_string() + "\"...");
            pair.second.destroy();
            Logger::writeToLog("Deleted SpriteFont");
        }
        Logger::unindent();
        Logger::writeToLog("Destroyed ContextData");
    }
}
