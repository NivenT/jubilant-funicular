#ifndef NTA_SYSTEMMANAGER_H_INCLUDED
#define NTA_SYSTEMMANAGER_H_INCLUDED

#include <map>

#include "nta/GLSLProgram.h"
#include "nta/Window.h"

namespace nta {
    /// Manages "System" stuff, whatever that means
    class SystemManager {
    private:
        /// Collection of GLSLProgram
        static std::map<std::string, GLSLProgram*> m_glslMap;
        /// Collection of Window
        static std::map<std::string, Window*> m_windowMap;
    public:
        /// Gets GLSLProgram at specified path
        /// Assumes vertex shader is (progPath + ".vert") and
        ///         fragment shader is (progPath + ".frag")
        static GLSLProgram* getGLSLProgram(crstring progPath);
        /// Gets GLSLProgram, specifying explicitly where the vertex and fragment
        ///   shaders are. Uses name as the key in the map
        static GLSLProgram* getGLSLProgram(crstring name, crstring vert, crstring frag);
        /// Gets a Window with specified title, width, height, and flags
        static Window* getWindow(crstring windowTitle, int width = 640, 
                                 int height = 480, int flags = 0);
        static Window* getWindow(crstring windowTitle, int flags) {
            return getWindow(windowTitle, 640, 480, flags);
        }
        /// Destorys the manager, freeing all necessary data
        static void destroy();
    };
}

#endif // NTA_SYSTEMMANAGER_H_INCLUDED
