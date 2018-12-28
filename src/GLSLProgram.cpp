#include <fstream>
#include <queue>

#include <GL/glew.h>

#include "nta/GLSLProgram.h"
#include "nta/Logger.h"

namespace nta {
    GLSLProgram::GLSLProgram() {
    }
    GLSLProgram::~GLSLProgram() {
        if (m_programID != 0) {
            glDeleteProgram(m_programID);
        }
        m_programID = 0;
    }
    GLuint GLSLProgram::compileShader(crstring shaderFileName, GLenum shaderType) const {
        GLuint retID = 0;
        retID = glCreateShader(shaderType);
        if (retID == 0) {
            Logger::writeErrorToLog("Failed to create shader", GL_FAILURE);
        }
        std::ifstream shaderFile(shaderFileName);
        if (shaderFile.fail()) {
            Logger::writeErrorToLog("Failed to open shader file: " + shaderFileName,
                                    MISSING_RESOURCE);
        }
        std::string fileContents = "";
        std::string line;
        while (getline(shaderFile, line)) {
            fileContents += line + '\n';
        }
        shaderFile.close();
        const char* contentsPtr = fileContents.c_str();
        glShaderSource(retID, 1, &contentsPtr, nullptr);
        glCompileShader(retID);
        GLint compileStatus = 0;
        glGetShaderiv(retID, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(retID, GL_INFO_LOG_LENGTH, &maxLength);
            char* log = new char[maxLength];
            glGetShaderInfoLog(retID, maxLength, &maxLength, log);
            if (maxLength > 0) {
                glDeleteShader(retID);
                Logger::writeToLog(shaderFileName + " log:");
                Logger::writeErrorToLog(log, GL_FAILURE);
            }
        }
        return retID;
    }
    GLint GLSLProgram::getUniformLocation(crstring uniformName) const {
        GLint ret = glGetUniformLocation(m_programID, uniformName.c_str());
        if (ret == GL_INVALID_INDEX) {
            Logger::writeErrorToLog("Could not find uniform in shader: " + uniformName,
                                    GL_FAILURE);
        }
        return ret;
    }
    bool GLSLProgram::isLinked() const {
        return m_isLinked;
    }
    void GLSLProgram::compileShaders(crstring shaderProgName) {
        Logger::writeToLog("Compiling shaders: " + shaderProgName + "...");
        m_programID = glCreateProgram();
        m_vert = shaderProgName + ".vert";
        m_vertShaderID = compileShader(m_vert.to_string(), GL_VERTEX_SHADER);
        m_frag = shaderProgName + ".frag";
        m_fragShaderID = compileShader(m_frag.to_string(), GL_FRAGMENT_SHADER);
        Logger::writeToLog("Compiled shaders");
    }
    void GLSLProgram::compileShaders(crstring vert, crstring frag) {
        Logger::writeToLog("Compiling shaders: " + vert + " | " + frag + "...");
        m_programID = glCreateProgram();
        std::tie(m_vert, m_frag) = std::make_tuple(vert, frag);
        m_vertShaderID = compileShader(vert, GL_VERTEX_SHADER);
        m_fragShaderID = compileShader(frag, GL_FRAGMENT_SHADER);
        Logger::writeToLog("Compiled shaders");
    }
    void GLSLProgram::addAttribute(crstring attributeName) {
        Logger::writeToLog("Adding attribute \"" + attributeName + "\" to program...");
        glBindAttribLocation(m_programID, m_numAttributes++, attributeName.c_str());
    }
    void GLSLProgram::linkShaders() {
        Logger::writeToLog("Linking shaders to program...");
        glAttachShader(m_programID, m_vertShaderID);
        glAttachShader(m_programID, m_fragShaderID);
        glLinkProgram(m_programID);
        GLint linkStatus;
        glGetProgramiv(m_programID, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);
            char* log = new char[maxLength];
            glGetProgramInfoLog(m_programID, maxLength, &maxLength, log);
            if (maxLength > 0) {
                glDeleteShader(m_vertShaderID);
                glDeleteShader(m_fragShaderID);
                glDeleteProgram(m_programID);
                Logger::writeToLog("GLSLProgram log:");
                Logger::writeErrorToLog(log, GL_FAILURE);
            }
        }
        glDetachShader(m_programID, m_vertShaderID);
        glDetachShader(m_programID, m_fragShaderID);
        glDeleteShader(m_vertShaderID);
        glDeleteShader(m_fragShaderID);
        m_isLinked = true;
        Logger::writeToLog("Linked shaders");
    }
    void GLSLProgram::use() const {
        glUseProgram(m_programID);
        for (int i = 0; i < m_numAttributes; i++) {
            glEnableVertexAttribArray(i);
        }
    }
    void GLSLProgram::unuse() const {
        glUseProgram(0);
        for (int i = 0; i < m_numAttributes; i++) {
            glDisableVertexAttribArray(i);
        }
    }
    void GLSLProgram::destroy() {
        glDeleteProgram(m_programID);
        m_programID = 0;
        m_numAttributes = 0;
        m_isLinked = false;
    }
    void GLSLProgram::reload() {
        static const std::size_t MAX_ATTRIB_NAME_SIZE = 64;
        Logger::writeToLog("Reloading shaders: " + m_vert.to_string() + " | " + m_frag.to_string() + "...");
        Logger::indent();

        std::vector<std::string> attribs(m_numAttributes);
        for (int i = 0; i < m_numAttributes; i++) {
            char attribName[MAX_ATTRIB_NAME_SIZE];
            GLint size;
            GLenum type;
            glGetActiveAttrib(m_programID, i, MAX_ATTRIB_NAME_SIZE, nullptr,
                              &size, &type, attribName);
            attribs[glGetAttribLocation(m_programID, attribName)] = attribName;
        }

        destroy();
        compileShaders(m_vert.to_string(), m_frag.to_string());
        for (auto& attrib : attribs) {
            addAttribute(attrib);
        }
        linkShaders();

        Logger::unindent();
        Logger::writeToLog("Reloaded shaders");
    }
}
