/// \todo prefix all include guards with NTA_
#ifndef NTA_MYENGINE_H_INCLUDED
#define NTA_MYENGINE_H_INCLUDED

#include <sstream>
#include <glm/glm.hpp>

#define NTA_ASSERT(expr, msg) assert(expr && __FILE__ ", line __LINE__" ": " msg)

/// \todo Throughout codebase, indent `case` inside of `switch` blocks
/// \todo Get a linter or something
/// \todo Decide what things you want to be CamelCase and which you want to snake_case
/// \todo Replace (almost) all raw pointers with (potentially custom) smart pointers
namespace nta {
	typedef const std::string& crstring; // constant reference string
	typedef const glm::ivec2& crivec2;
	typedef const glm::vec2& crvec2;
	typedef const glm::vec3& crvec3;
	typedef const glm::vec4& crvec4;

    /// initializes some basic stuff for the engine
    extern void init(int gl_major_version = 3, int gl_minor_version = 2, bool use_gl_core = true);
    /// deinitializes some stuff and destroys all managers
    extern void cleanup();
    /// Checks for and logs errors
    extern bool check_error();
    /// Lock stream for thread-safe manipulation
    extern std::ostream& lock_stream(std::ostream& stream);
    /// Unlock stream
    extern std::ostream& unlock_stream(std::ostream& stream);
};

#endif // NTA_MYENGINE_H_INCLUDED
