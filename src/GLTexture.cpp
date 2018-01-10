#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

#include "GLTexture.h"
#include "Logger.h"

namespace nta {
    GLTexture ImageLoader::readImage(crstring filePath, GLint minFilt,
                                     GLint magFilt, crvec2 dimensions) {
        Logger::writeToLog("Loading image: " + filePath + "...");
        GLTexture ret;
        ILuint imgID = 0;
        ilGenImages(1,&imgID);
        ilBindImage(imgID);
        if (ilLoadImage(filePath.c_str()) == IL_FALSE) {
            ILenum error = ilGetError();
            Logger::writeErrorToLog("DevIL failed to load image with error " +
                                    to_string(error) + ": " + iluErrorString(error));
        }
        glGenTextures(1, &ret.id);
        glBindTexture(GL_TEXTURE_2D, ret.id);
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        if (dimensions != glm::vec2(0)) {
            iluScale(dimensions.x, dimensions.y, 1);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     (GLubyte*)ilGetData());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        ret.width = ilGetInteger(IL_IMAGE_WIDTH);
        ret.height = ilGetInteger(IL_IMAGE_HEIGHT);
        Logger::writeToLog("Loaded image (" + to_string(ret.width) + " x " +
                           to_string(ret.height) + ")");
        return ret;
    }
}
