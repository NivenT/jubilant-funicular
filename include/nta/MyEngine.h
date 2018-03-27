#ifndef MYENGINE_H_INCLUDED
#define MYENGINE_H_INCLUDED

#include <sstream>
#include <glm/glm.hpp>

typedef const std::string& crstring; // constant reference string
typedef const glm::vec2& crvec2;
typedef const glm::vec3& crvec3;
typedef const glm::vec4& crvec4;
namespace nta {
    /// initializes some basic stuff for the engine
    extern void init();
    /// Rotates a point (about the origin) by the given angle
    extern glm::vec2 rotate(crvec2 pt, float angle);

    /// converts input to an std::string
    template<class T>
    std::string to_string(T input, std::size_t precision = 0) {
        std::ostringstream os;
        if (precision != 0) {
            os.precision(precision);
        }
        os<<input;
        return os.str();
    }

    /// returns whether or not min <= val <= max
    template<class T>
    bool in_range(T val, T min, T max) {
        return min <= val && val <= max;
    } 
};

#endif // MYENGINE_H_INCLUDED
