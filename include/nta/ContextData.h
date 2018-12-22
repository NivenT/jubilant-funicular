#ifndef NTA_CONTEXTDATAMANAGER_H_INCLUDED
#define NTA_CONTEXTDATAMANAGER_H_INCLUDED

#include <map>

#include "nta/GLSLProgram.h"
#include "nta/GLTexture.h"
#include "nta/SpriteFont.h"
#include "nta/Path.h"

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
        std::map<utils::Path, GLTexture> m_textureMap;
        /// Inverse map to m_textureMap
        std::map<GLTexture, utils::Path> m_textureFiles;
        /// Map front (font name, font size) -> SpriteFont
        std::map<std::pair<utils::Path, int>, SpriteFont> m_fontMap;
        /// Folder containing textures
        utils::Path m_texture_folder = "./";
        /// Folder containing shaders
        utils::Path m_glsl_folder = "./";
        /// Folder containing fonts
        utils::Path m_font_folder = "./";
    public:
        /// Gets GLSLProgram at specified path
        ///
        /// Assumes vertex shader is (progPath + ".vert") and
        /// fragment shader is (progPath + ".frag")
        GLSLProgram* getGLSLProgram(const utils::Path& progPath);
        /// Gets GLSLProgram, specifying explicitly where the vertex and fragment
        /// shaders are. 
        ///
        /// Uses name as the key in the map
        GLSLProgram* getGLSLProgram(crstring name, crstring vert, crstring frag);
        /// Gets a GLTexture representing the image at the given path
        Result<GLTexture> getTexture(const utils::Path& path, crvec2 dimensions = glm::vec2(0));
        /// Gets the name of the file used the create tex
        Result<utils::Path> getTextureFile(GLTexture tex);
        /// Gets SpriteFont created from the given font with the given size
        Result<SpriteFont*> getSpriteFont(const utils::Path& fontPath, int fontSize = 32);
        /// Sets the texture folder
        void setTextureFolder(const utils::Path& fldr) { m_texture_folder = fldr; }
        /// Sets the shader folder
        void setGLSLFolder(const utils::Path& fldr) { m_glsl_folder = fldr; }
        /// Sets the font folder
        void setFontFolder(const utils::Path& fldr) { m_font_folder = fldr; }
        /// Deletes all data (does not reset folder paths)
        void destroy();
    };
}

#endif // NTA_CONTEXTDATAMANAGER_H_INCLUDED
