#ifdef NTA_USE_DEVIL
    #include <IL/il.h>
    #include <IL/ilu.h>
#else
    #include <CImg.h>
#endif

#include "nta/GLTexture.h"
#include "nta/Logger.h"
#include "nta/utils.h"

namespace nta {
    GLTexture::GLTexture(const RawTexture& raw, GLint minFilt, GLint magFilt) : 
        width(raw.width), height(raw.height) {
        Logger::writeToLog("Creating GLTexture from RawTexture...");
        if (raw.data) {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw.width, raw.height, 0,
                         raw.format, GL_UNSIGNED_BYTE, raw.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilt);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            Logger::writeToLog("Created GLTexture with id " + utils::to_string(id));
        } else {
            Logger::writeToLog("The RawTexture was empty");
        }
    }
    void GLTexture::free() {
        Logger::writeToLog("Freeing GLTexture with id " + utils::to_string(id));
        glDeleteTextures(1, &id);
        Logger::writeToLog("Freed GLTexture");
    }

    Result<RawTexture> ImageLoader::readImage(crstring filePath, crvec2 dimensions) {
        Logger::writeToLog("Loading image \"" + filePath + "\" into RawTexture...");
        Logger::indent();
        #ifdef NTA_USE_DEVIL
            ILuint imgID = 0;
            ilGenImages(1, &imgID);
            ilBindImage(imgID);
            if (ilLoadImage(filePath.c_str()) == IL_FALSE) {
                ILenum error = ilGetError();
                // This is some jank identing
                auto err = Logger::writeErrorToLog(
                            "DevIL failed to load image with error " +
                                utils::to_string(error) + ": " + iluErrorString(error),
                            DEVIL_FAILURE);
                return Result<RawTexture>::new_err(err);
            }
            ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        #else
            cimg_library::CImg<GLubyte> image(filePath.c_str());
        #endif

        RawTexture ret;
        if (dimensions != glm::vec2(0)) {
            #ifdef NTA_USE_DEVIL
                iluScale(dimensions.x, dimensions.y, 1);
            #else
                image = image.resize(dimensions.x, dimensions.y);
            #endif
        }
        #ifdef NTA_USE_DEVIL
            ret.width = ilGetInteger(IL_IMAGE_WIDTH);
            ret.height = ilGetInteger(IL_IMAGE_HEIGHT);
            ret.format = GL_RGBA;

            ret.data = new GLubyte[ret.width*ret.height*4];
            memcpy(ret.data, ilGetData(), ret.width*ret.height*4);
        #else
            ret.width = image.width();
            ret.height = image.height();
            // I'd prefer it if I could convert any image to RGBA
            ret.format = image.spectrum() == 3 ? GL_RGB : GL_RGBA;

            // CImg stores images in a stupid way; this fixes that
            image.permute_axes("cxyz");
            ret.data = new GLubyte[ret.width*ret.height*image.spectrum()];
            memcpy(ret.data, image.data(), ret.width*ret.height*image.spectrum());
        #endif
        Logger::unindent();
        Logger::writeToLog("Loaded image");
        return Result<RawTexture>::new_ok(ret);
    }
    Result<GLTexture> ImageLoader::readImage(crstring filePath, GLint minFilt,
                                             GLint magFilt, crvec2 dimensions) {
        return readImage(filePath, dimensions).map<GLTexture>([&](const RawTexture& raw) {
            return GLTexture(raw, minFilt, magFilt);
        });
    }
}
