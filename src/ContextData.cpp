#include "nta/ContextData.h"
#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    GLSLProgram* ContextData::getGLSLProgram(crstring progPath) {
        if (m_glslMap.find(progPath) == m_glslMap.end()) {
            m_glslMap[progPath].compileShaders(progPath);
        }
        return &m_glslMap[progPath];
    }
    GLSLProgram* ContextData::getGLSLProgram(crstring name, crstring vert, crstring frag) {
        if (m_glslMap.find(name) == m_glslMap.end()) {
            m_glslMap[name].compileShaders(vert, frag);
        }
        return &m_glslMap[name];
    }
    Result<GLTexture> ContextData::getTexture(crstring path) {
        if (m_textureMap.find(path) == m_textureMap.end()) {
            auto raw = ResourceManager::getTexture(path);
            // I really like this bit
            return raw.map<GLTexture>([&](const RawTexture& raw) {
                Logger::writeToLog("RawTexture \"" + path + "\" doesn't have a GLTexture in this context.");
                return m_textureMap[path] = GLTexture(raw);
            });
        }
        return Result<GLTexture>::new_ok(m_textureMap[path]);
    }
    Result<std::string> ContextData::getTextureFile(GLTexture tex) {
        if (m_textureFiles.find(tex) == m_textureFiles.end()) {
            auto err = Logger::writeErrorToLog("tex not found in ContextData::m_textureFiles", MISSING_RESOURCE);
            return Result<std::string>::new_err(err);
        }
        return Result<std::string>::new_ok(m_textureFiles[tex]);
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
            Logger::writeToLog("Deleting GLTexture \"" + pair.first + "\"...");
            pair.second.destroy();
            Logger::writeToLog("Deleted GLTexture");
        }
        Logger::unindent();
        Logger::writeToLog("Destroyed ContextData");
    }
}
