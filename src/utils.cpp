#include "nta/utils.h"

namespace nta {
    namespace utils {
        bool starts_with(crstring str, crstring prefix) {
            return ::utils::starts_with(str, prefix);
        }
        bool ends_with(crstring str, crstring suffix) {
            return ::utils::ends_with(str, suffix);
        }
        std::string replace_all(crstring str, crstring o, crstring n) {
            return ::utils::replace_all(str, o, n);
        }
        std::string replace_all(crstring str, const std::vector<std::vector<std::string>>& os, const std::vector<std::string>& ns) {
            return ::utils::replace_all(str, os, ns);
        }
        std::string trim(crstring str, crstring back_trash, crstring front_trash) {
            return ::utils::trim(str, back_trash, front_trash);
        }
        std::vector<std::string> split(crstring str, char delim) {    
            return ::utils::split(str, delim); 
        }
        glm::vec2 rotate(crvec2 pt, float angle) {
        return glm::vec2(glm::cos(angle)*pt.x - glm::sin(angle)*pt.y,
                             glm::sin(angle)*pt.x + glm::cos(angle)*pt.y);
        }
    }
}