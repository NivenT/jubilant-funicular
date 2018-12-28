#ifndef NTA_GLSLPROGRAM_H_INCLUDED
#define NTA_GLSLPROGRAM_H_INCLUDED

#include "nta/MyEngine.h"
#include "nta/Path.h"

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int GLint;

namespace nta {
    class ContextData;
    /// represents a program written in GLSL comprised of a vertex shader and a fragment shader
    class GLSLProgram {
    private:
        /// compiles both the vertex and fragment shaders given their name
        void compileShaders(crstring shaderProgName);
        void compileShaders(crstring vert, crstring frag);
        /// compiles a shader from a file and returns its id
        GLuint compileShader(crstring shaderFileName, GLenum shaderType) const;
        /// the id for the program
        GLuint m_programID = 0;
        /// the ids for the vertex and fragment shaders
        GLuint m_vertShaderID = 0, m_fragShaderID = 0;
        /// The files containing the vertex and fragment shaders
        utils::Path m_vert, m_frag;
        /// the number of attributes used by the vertex shader
        int m_numAttributes = 0;
        /// keeps track of whether or not the shaders have been linked
        bool m_isLinked = false;
    public:
        /// constructor and destructor
        GLSLProgram();
        ~GLSLProgram();
        /// returns the location of a uniform in the shaders
        GLint getUniformLocation(crstring uniformName) const;
        /// returns whether or not the shaders have been linked
        bool isLinked() const;
        /// makes an attribute useful and assigns it the next available location
        void addAttribute(crstring attributeName);
        /// links the compiled shaders to this program
        void linkShaders();
        /// binds this program
        void use() const;
        /// unbinds this program
        void unuse() const;
        /// deletes this program
        void destroy();
        /// Reloads the program
        void reload();
        friend class ContextData;
    };
}

#endif // NTA_GLSLPROGRAM_H_INCLUDED
