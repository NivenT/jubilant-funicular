#include <CImg.h>
#include <GL/glew.h>

#include "nta/GLTexture.h"
#include "nta/Logger.h"

namespace nta {
    GLTexture ImageLoader::readImage(crstring filePath, GLint minFilt,
                                     GLint magFilt, crvec2 dimensions) {
        Logger::writeToLog("Loading image: " + filePath + "...");
        cimg_library::CImg<GLubyte> image(filePath.c_str());

        GLTexture ret;
        glGenTextures(1, &ret.id);
        glBindTexture(GL_TEXTURE_2D, ret.id);
        if (dimensions != glm::vec2(0)) {
            image = image.resize(dimensions.x, dimensions.y);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        ret.width = image.width();
        ret.height = image.height();
        Logger::writeToLog("Loaded image (" + to_string(ret.width) + " x " +
                           to_string(ret.height) + ")");
        return ret;
    }
}
