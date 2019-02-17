#include <algorithm>
#include <numeric>
#include <vector>
#include <tuple>

#include "nta/Camera2D.h"

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
    glm::mat3 Camera2D::getRotationMatrix() const {
        return glm::mat3(
             glm::cos(m_orientation), glm::sin(m_orientation), 0,
            -glm::sin(m_orientation), glm::cos(m_orientation), 0,
            0,                        0,                       1
        );
    }
    glm::mat3 Camera2D::getInverseRotationMatrix() const {
        return glm::mat3(
            glm::cos(m_orientation), -glm::sin(m_orientation), 0,
            glm::sin(m_orientation),  glm::cos(m_orientation), 0,
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
        return getDilationMatrix() * getRotationMatrix() * getTranslationMatrix();
    }
    glm::mat3 Camera2D::getInverseCameraMatrix() const {
        return glm::inverse(getCameraMatrix());
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
        glm::vec2 axes = getRotatedDimensions();
        return glm::vec2(m_center.x-axes.x,m_center.y+axes.y);
    }
    glm::vec2 Camera2D::getDimensions() const {
        return m_dimensions;
    }
    glm::vec2 Camera2D::getRotatedDimensions() const {
        glm::vec3 axes = getRotationMatrix() * glm::vec3(m_dimensions, 1.);
        return glm::vec2(axes.x, axes.y);
    }
    float Camera2D::getOrientation() const {
        return m_orientation;
    }
    std::tuple<glm::vec2, glm::vec2> Camera2D::getAxes() const {
        auto mat = getRotationMatrix();
        glm::vec3 e1 = mat * glm::vec3(1,0,1), e2 = mat * glm::vec3(0,1,1);
        return std::make_tuple(glm::vec2(e1.x, e1.y), glm::vec2(e2.x, e2.y));
    }
    glm::vec2 Camera2D::mouseToGame(crvec2 mouse, crvec2 windowDimensions) const {
        // [a,b] -> [0,b-a] -> [0,d-c] -> [c,d]
        glm::vec2 screen(mouse.x, windowDimensions.y - mouse.y);
        screen = 2.f*screen/windowDimensions - 1.f;
        return screenToGame(screen);
    }
    glm::vec2 Camera2D::screenToGame(crvec2 screen) const {
        glm::vec3 orig = getInverseCameraMatrix() * glm::vec3(screen, 1);
        return glm::vec2(orig.x, orig.y);
    }
    bool Camera2D::inBounds(crvec2 pt) const {
        glm::vec2 transformed_pt = getInverseRotationMatrix() * glm::vec3(pt - m_center, 1);
        return -m_dimensions.x <= transformed_pt.x && transformed_pt.x <= m_dimensions.x &&
               -m_dimensions.y <= transformed_pt.y && transformed_pt.y <= m_dimensions.y;
    }
    bool Camera2D::inBounds(float x, float y) const {
        return inBounds(glm::vec2(x, y));
    }
    // There's gotta be a better way to do this
    bool Camera2D::isVisible(const std::vector<glm::vec2>& polygon) const {
        std::vector<glm::vec2> transformed_poly(polygon.size());
        std::transform(polygon.begin(), polygon.end(), transformed_poly.begin(), [&](crvec2 pt) {
            return getInverseRotationMatrix() * glm::vec3(pt - m_center, 1);
        });

        std::vector<glm::vec2> normals(polygon.size()+2);
        normals[0] = glm::vec2(1,0);
        normals[1] = glm::vec2(0,1);
        for (int i = 0; i < polygon.size(); i++) {
            glm::vec2& p1 = transformed_poly[i];
            glm::vec2& p2 = transformed_poly[(i+1)%polygon.size()];
            glm::vec2 side = p2 - p1;

            normals[i+2] = glm::vec2(-side.y, side.x);
        }

        glm::vec2 cam_pts[4] = {
            glm::vec2(-m_dimensions.x, m_dimensions.y),
            glm::vec2(m_dimensions.x, m_dimensions.y),
            glm::vec2(m_dimensions.x, -m_dimensions.y),
            glm::vec2(-m_dimensions.x, -m_dimensions.y)
        };
        for (const auto& n : normals) {
            auto cam_minmax = std::accumulate(cam_pts, cam_pts+4, std::make_pair(FLT_MAX, FLT_MIN), 
                                              [&](std::pair<float, float> p, crvec2 pt) {
              float proj = glm::dot(n, pt);
              p.first = std::min(p.first, proj);
              p.second = std::max(p.second, proj);
              return p;
            });
            auto poly_minmax = std::accumulate(transformed_poly.begin(), transformed_poly.end(), std::make_pair(FLT_MAX, FLT_MIN), 
                                              [&](std::pair<float, float> p, crvec2 pt) {
              float proj = glm::dot(n, pt);
              p.first = std::min(p.first, proj);
              p.second = std::max(p.second, proj);
              return p;
            });

            if (cam_minmax.first > poly_minmax.second || 
                poly_minmax.first > cam_minmax.second) {
                return false;
            }
        }
        return true;
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
            m_center += translation;
        } else {
            glm::vec3 move = getInverseRotationMatrix() * glm::vec3(translation, 1.);
            m_center += glm::vec2(move.x, move.y); // Probably an easier way to do this
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
