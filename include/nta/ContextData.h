#ifndef NTA_CONTEXTDATAMANAGER_H_INCLUDED
#define NTA_CONTEXTDATAMANAGER_H_INCLUDED

#include <map>
#include <unordered_map>

#include "nta/GLSLProgram.h"
#include "nta/GLTexture.h"
#include "nta/SpriteFont.h"

namespace nta {
    /// Certain pieces of data are tied to a specific GL context (e.g. a Window).
    /// This class manages all such data specific to one context.
    ///
    /// Generally, there should be one ContextData per ScreenManager.
    // \question Should these things have a unique ID for logging purposes?
    class ContextData {
    private:
        /// Collection of GLSLProgram
        std::map<std::string, GLSLProgram> m_glslMap;
        /// a map file names to textures
        std::map<std::string, GLTexture> m_textureMap;
        /// Inverse map to m_textureMap
        std::unordered_map<GLTexture, std::string> m_textureFiles;
        /// Map front (font name, font size) -> SpriteFont
        std::map<std::pair<std::string, int>, SpriteFont> m_fontMap;
    public:
        /// Gets GLSLProgram at specified path
        ///
        /// Assumes vertex shader is (progPath + ".vert") and
        /// fragment shader is (progPath + ".frag")
        GLSLProgram* getGLSLProgram(crstring progPath);
        /// Gets GLSLProgram, specifying explicitly where the vertex and fragment
        /// shaders are. 
        ///
        /// Uses name as the key in the map
        GLSLProgram* getGLSLProgram(crstring name, crstring vert, crstring frag);
        /// Gets a GLTexture representing the image at the given path
        Result<GLTexture> getTexture(crstring path);
        /// Gets the name of the file used the create tex
        Result<std::string> getTextureFile(GLTexture tex);
        /// Gets SpriteFont created from the given font with the given size
        Result<SpriteFont*> getSpriteFont(crstring fontPath, int fontSize = 32);
        /// Deletes all data
        void destroy();
    };
}

#endif // NTA_CONTEXTDATAMANAGER_H_INCLUDED
