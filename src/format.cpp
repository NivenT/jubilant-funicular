#include <nta/format.h>

namespace nta {
    namespace utils{
        std::string format_helper(std::string& sofar, const std::string& fmt) {
            return sofar + fmt;
        }
    }
}
