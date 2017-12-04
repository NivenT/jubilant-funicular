#ifndef MYENGINE_H_INCLUDED
#define MYENGINE_H_INCLUDED

#include <functional>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

typedef const std::string& crstring; //constant reference string
typedef const glm::vec2& crvec2;
typedef const glm::vec3& crvec3;
typedef const glm::vec4& crvec4;
namespace nta {
    ///initializes some basic stuff for the engine
    extern void init();
};
///transforms a value in the range [a,b] to a value in the range [c,d]
extern float changeRange(float val, float a, float b, float c, float d);
///Calls the first function corresponding to a true statement
extern void cond(std::initializer_list<bool> conditions, std::initializer_list<std::function<void()>> exprs);

// Shoudl these be in namespace nta?

///converts input to an std::string
template<class T>
std::string to_string(T input, std::size_t precision = 0) {
    std::ostringstream os;
    if (precision != 0)
        os.precision(precision);
    os<<input;
    return os.str();
}

///Returns the first value corresponding to a true statement
template<class T>
T cond(std::initializer_list<bool> conditions, std::initializer_list<T> vals) {
    for (int i = 0; i < conditions.size(); i++) {
        if (*(conditions.begin()+i)) {
            return *(vals.begin()+i);
        }
    }
    return T();
}

///returns whether or not min <= val <= max
template<class T>
bool inRange(T val, T min, T max) {
    return (min <= val && val <= max);
}
#endif // MYENGINE_H_INCLUDED
