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
    /// Checks for and logs errors
    extern bool check_error();
};

#endif // NTA_MYENGINE_H_INCLUDED
