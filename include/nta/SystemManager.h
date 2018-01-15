#ifndef SYSTEMMANAGER_H_INCLUDED
#define SYSTEMMANAGER_H_INCLUDED

#include <map>

#include "nta/GLSLProgram.h"
#include "nta/Window.h"

namespace nta {
    class SystemManager {
    private:
        static std::map<std::string, GLSLProgram*> m_glslMap;
        static std::map<std::string, Window*> m_windowMap;
    public:
        static GLSLProgram* getGLSLProgram(crstring progPath);
        static GLSLProgram* getGLSLProgram(crstring name, crstring vert, crstring frag);
        static Window* getWindow(crstring windowTitle, int flags = 0);
        static void destroy();
    };
}

#endif // SYSTEMMANAGER_H_INCLUDED
