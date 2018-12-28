#ifndef CAMERA2D_H_INCLUDED
#define CAMERA2D_H_INCLUDED

#include "MyEngine.h"

namespace nta {
    /// represents a camera in two dimensions from which the world is viewed
    ///
    /// You can imagine the 2D world is flat against the wall, and this camera is 
    /// facing that wall. You can move the camera in space (i.e. setCenter/setDimensions),
    /// and it can be rotated about an axis orthogonal to the world, through its center. 
    /// \todo Rename m_dimensions to union { m_extents; m_half_dimensions; }
    class Camera2D {
    private:
        /// center of the camera's view in world coordinates
        glm::vec2 m_center;
        /// half dimensions of camera's view in world coordinates
        glm::vec2 m_dimensions;
        /// rotation about axis orthogonal to the world
        float m_orientation;
    public:
        /// Constructs a new Camera2D. By default, the center is at
        /// (0,0), the dimensions are (100, 100), and orientation is 0
        Camera2D();
        Camera2D(crvec2 center);
        Camera2D(crvec2 center, crvec2 dimensions);
        Camera2D(crvec2 center, crvec2 dimensions, float orientation);
        /// destructor
        ~Camera2D();
        /// returns the 3x3 matrix representing the camera's view
        glm::mat3 getTranslationMatrix() const;
        glm::mat3 getRotationMatrix() const;
        glm::mat3 getInverseRotationMatrix() const;
        glm::mat3 getDilationMatrix() const;
        glm::mat3 getCameraMatrix() const;
        glm::mat3 getInverseCameraMatrix() const;
        /// returns camera bounds in the given format
        glm::vec4 getBoundsCenter() const;
        glm::vec4 getBoundsTopLeft() const;
        /// returns the center, top left coordinate, and (half) dimensions of the camera's view
        glm::vec2 getCenter() const;
        glm::vec2 getTopLeft() const;
        glm::vec2 getDimensions() const;
        glm::vec2 getRotatedDimensions() const;
        float getOrientation() const;
        /// returns (normalized) axes aligned with the camera
        std::tuple<glm::vec2, glm::vec2> getAxes() const;
        /// converts mouse coordinates to world coordinates
        glm::vec2 mouseToGame(crvec2 mouse, crvec2 windowDimensions) const;
        /// converts screen coordinates to world coordinates
        ///
        /// screen here just means the coordinates after multiplying by the camera matrix
        /// this is usually what's passed into gl_Position
        glm::vec2 screenToGame(crvec2 screen) const;
        /// sets the values of the camera's fields
        void setCenter(crvec2 center);
        void setCenter(float x, float y);
        void setDimensions(crvec2 dimensions);
        void setDimensions(float w, float h);
        void setOrientation(float t);
        /// moves the camera around the world
        /// By default, if the world is rotated, it does not move along the rotated axes
        void translateCenter(crvec2 translation, bool move_along_axes = false);
        void translateCenter(float dx, float dy, bool move_along_axes = false);
        /// scales the camera's field of view
        void scaleDimensions(crvec2 dilation);
        void scaleDimensions(float dw, float dh);
        /// rotates the camera
        void rotate(float dt);
    };
}

#endif // CAMERA2D_H_INCLUDED
