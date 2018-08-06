/// \todo prefix all include guards with NTA_
#ifndef NTA_MYENGINE_H_INCLUDED
#define NTA_MYENGINE_H_INCLUDED

#include <sstream>
#include <glm/glm.hpp>

typedef const std::string& crstring; // constant reference string
typedef const glm::ivec2& crivec2;
typedef const glm::vec2& crvec2;
typedef const glm::vec3& crvec3;
typedef const glm::vec4& crvec4;
/// \todo Throughout codebase, indent `case` inside of `switch` blocks
namespace nta {
    /// initializes some basic stuff for the engine
    extern void init(int gl_major_version = 3, int gl_minor_version = 2, bool use_gl_core = true);
    /// deinitializes some stuff and destroyes all managers
    extern void cleanup();
    /// Rotates a point (about the origin) by the given angle
    extern glm::vec2 rotate(crvec2 pt, float angle);
    /// Checks for and logs errors
    extern bool check_error();
    /* If you're compiling with clang, uncomment this and then have fun trying to get things to work
    // Not sure how I feel about this but brought about by clang being stupid
    extern std::ostream& operator<<(std::ostream&, crvec2);
    extern std::ostream& operator<<(std::ostream&, crvec4);
    extern std::ostream& operator<<(std::ostream&, const glm::mat3&);
    */
    /// converts input to a std::string
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

#endif // NTA_MYENGINE_H_INCLUDED
