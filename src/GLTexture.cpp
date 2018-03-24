#ifdef USE_DEVIL
    #include <IL/il.h>
    #include <IL/ilu.h>
#else
    #include <CImg.h>
#endif
#include <GL/glew.h>

#include "nta/GLTexture.h"
#include "nta/Logger.h"

namespace nta {
    GLTexture ImageLoader::readImage(crstring filePath, GLint minFilt,
                                     GLint magFilt, crvec2 dimensions) {
        Logger::writeToLog("Loading image: " + filePath + "...");
        #ifdef USE_DEVIL
            ILuint imgID = 0;
            ilGenImages(1, &imgID);
            ilBindImage(imgID);
            if (ilLoadImage(filePath.c_str()) == IL_FALSE) {
                ILenum error = ilGetError();
                Logger::writeErrorToLog("DevIL failed to load image with error " +
                                        to_string(error) + ": " + iluErrorString(error));
            }
            ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        #else
            cimg_library::CImg<GLubyte> image(filePath.c_str());
        #endif

        GLTexture ret;
        glGenTextures(1, &ret.id);
        glBindTexture(GL_TEXTURE_2D, ret.id);
        if (dimensions != glm::vec2(0)) {
            #ifdef USE_DEVIL
                iluScale(dimensions.x, dimensions.y, 1);
            #else
                image = image.resize(dimensions.x, dimensions.y);
            #endif
        }
        #ifdef USE_DEVIL
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     (GLubyte*)ilGetData());
        #else
            // I'd prefer it if I could convert any image to RGBA
            GLint format = image.spectrum() == 3 ? GL_RGB : GL_RGBA;
            ret.width = image.width();
            ret.height = image.height();
            // CImg stores images in a stupid way; this fixes that
            image.permute_axes("cxyz");
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret.width, ret.height,
                         0, format, GL_UNSIGNED_BYTE, (GLubyte*)image.data());
        #endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        #ifdef USE_DEVIL
            ret.width = ilGetInteger(IL_IMAGE_WIDTH);
            ret.height = ilGetInteger(IL_IMAGE_HEIGHT);
        #endif
        Logger::writeToLog("Loaded image (" + to_string(ret.width) + " x " +
                           to_string(ret.height) + ")");
        return ret;
    }
}
