#ifndef CAMERA2D_H
#define CAMERA2D_H

#include "MyEngine.h"

namespace nta {
    ///represents a camera in two dimensions from which the world is viewed
    class Camera2D {
    private:
        ///center of the camera's view in world coordinates
        glm::vec2 m_center;
        ///half dimensions of camera's view in world coordinates
        glm::vec2 m_dimensions;
    public:
        ///constructors
        Camera2D();
        Camera2D(crvec2 center);
        Camera2D(crvec2 center, crvec2 dimensions);
        ///destructor
        ~Camera2D();
        ///returns the 3x3 matrix representing the camera's view
        glm::mat3 getCameraMatrix() const;
        ///returns camera bounds in the given format
        glm::vec4 getBoundsCenter() const;
        glm::vec4 getBoundsTopLeft() const;
        ///returns the center, top left coordinate, and dimensions of the camera's view
        glm::vec2 getCenter() const;
        glm::vec2 getTopLeft() const;
        glm::vec2 getDimensions() const;
        ///converts mouse coordinates to world coordinates
        glm::vec2 mouseToGame(crvec2 mouse, crvec2 windowDimensions) const;
        ///sets the values of the camera's fields
        void setCenter(crvec2 center);
        void setCenter(float x, float y);
        void setDimensions(crvec2 dimensions);
        void setDimensions(float w, float h);
        ///moves the camera around the world
        void translateCenter(crvec2 translation);
        void translateCenter(float dx, float dy);
        ///scales the camera's field of view
        void scaleDimensions(crvec2 dilation);
        void scaleDimensions(float dw, float dh);
    };
}

#endif // CAMERA2D_H
