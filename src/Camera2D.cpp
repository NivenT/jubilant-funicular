#include "Camera2D.h"

namespace nta {
    Camera2D::Camera2D() {
        m_center.x = m_center.y = 0;
        m_dimensions.x = m_dimensions.y = 100;
        m_orientation = 0;
    }
    Camera2D::Camera2D(crvec2 center) {
        m_center.x = center.x;
        m_center.y = center.y;
        m_dimensions.x = m_dimensions.y = 100;
        m_orientation = 0;
    }
    Camera2D::Camera2D(crvec2 center, crvec2 dimensions) {
        m_center.x = center.x;
        m_center.y = center.y;
        m_dimensions.x = dimensions.x;
        m_dimensions.y = dimensions.y;
        m_orientation = 0;
    }
    Camera2D::Camera2D(crvec2 center, crvec2 dimensions, float orientation) : 
        m_center(center), m_dimensions(dimensions), m_orientation(orientation) {
    }
    Camera2D::~Camera2D() {
        m_center.x = m_center.y = m_dimensions.x = m_dimensions.y = m_orientation = 0;
    }
    // glm takes matrices in column-major order
    glm::mat3 Camera2D::getTranslationMatrix() const {
        return glm::mat3(
            1,           0,           0,
            0,           1,           0,
            -m_center.x, -m_center.y, 1
        );
    }
    glm::mat3 Camera2D::getRotationMatrix(float angle) const {
        angle = std::isnan(angle) ? m_orientation : angle;
        return glm::mat3(
             glm::cos(angle), glm::sin(angle), 0,
            -glm::sin(angle), glm::cos(angle), 0,
            0,                        0,                       1
        );
    }
    glm::mat3 Camera2D::getDilationMatrix() const {
        return glm::mat3(
            1.f/m_dimensions.x, 0, 0,
            0, 1.f/m_dimensions.y, 0,
            0, 0,                  1
        );
    }
    glm::mat3 Camera2D::getCameraMatrix() const {
        return getDilationMatrix() * getTranslationMatrix() * getRotationMatrix();
    }
    glm::vec4 Camera2D::getBoundsCenter() const {
        return glm::vec4(m_center, m_dimensions);
    }
    glm::vec4 Camera2D::getBoundsTopLeft() const {
        return glm::vec4(m_center.x-m_dimensions.x, m_center.y+m_dimensions.y, 2.f*m_dimensions);
    }
    glm::vec2 Camera2D::getCenter() const {
        return m_center;
    }
    glm::vec2 Camera2D::getTopLeft() const {
        return glm::vec2(m_center.x-m_dimensions.x,m_center.y+m_dimensions.y);
    }
    glm::vec2 Camera2D::getDimensions() const {
        return m_dimensions;
    }
    float Camera2D::getOrientation() const {
        return m_orientation;
    }
    glm::vec2 Camera2D::mouseToGame(crvec2 mouse, crvec2 windowDimensions) const {
        /// [a,b]->[0,b-a]->[0,d-c]->[c,d]
        glm::vec2 ret(mouse.x, windowDimensions.y - mouse.y);
        ret *= 2.f*m_dimensions/windowDimensions;
        ret += m_center - m_dimensions;

        glm::vec3 true_ret = getRotationMatrix(-m_orientation) * glm::vec3(ret, 1);
        return glm::vec2(true_ret.x, true_ret.y);
    }
    void Camera2D::setCenter(crvec2 center) {
        m_center = center;
    }
    void Camera2D::setCenter(float x, float y) {
        m_center.x = x;
        m_center.y = y;
    }
    void Camera2D::setDimensions(crvec2 dimensions) {
        m_dimensions = dimensions;
    }
    void Camera2D::setDimensions(float w, float h) {
        m_dimensions.x = w;
        m_dimensions.y = h;
    }
    void Camera2D::setOrientation(float t) {
        m_orientation = t;
    }
    void Camera2D::translateCenter(crvec2 translation, bool move_along_axis) {
        if (move_along_axis) {
            glm::vec3 move = getRotationMatrix() * glm::vec3(translation, 1.);
            m_center += glm::vec2(move.x, move.y); // Probably an easier way to do this
        } else {
            m_center += translation;
        }
    }
    void Camera2D::translateCenter(float dx, float dy, bool move_along_axis) {
        translateCenter(glm::vec2(dx, dy), move_along_axis);
    }
    void Camera2D::scaleDimensions(crvec2 dilation) {
        m_dimensions *= dilation;
    }
    void Camera2D::scaleDimensions(float dw, float dh) {
        m_dimensions.x *= dw;
        m_dimensions.y *= dh;
    }
    void Camera2D::rotate(float dt) {
        m_orientation += dt;
    }
}
