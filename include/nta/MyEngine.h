#ifndef MYENGINE_H_INCLUDED
#define MYENGINE_H_INCLUDED

#include <functional>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

typedef const std::string& crstring; // constant reference string
typedef const glm::vec2& crvec2;
typedef const glm::vec3& crvec3;
typedef const glm::vec4& crvec4;

// TODO: Remove
#include <GLMConsoleOutput.h>

namespace nta {
    /// initializes some basic stuff for the engine
    extern void init();

    /// converts input to an std::string
    template<class T>
    std::string to_string(T input, std::size_t precision = 0) {
        std::ostringstream os;
        if (precision != 0)
            os.precision(precision);
        os<<input;
        return os.str();
    }

    /// returns whether or not min <= val <= max
    template<class T>
    bool in_range(T val, T min, T max) {
        return (min <= val && val <= max);
    } 
};

#endif // MYENGINE_H_INCLUDED
