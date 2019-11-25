#ifndef NTA_FORMATTER_H_INCLUDED
#define NTA_FORMATTER_H_INCLUDED

#include <string>
#include <sstream>

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// Specialize this struct to use custom types with format
        template<typename T>
        struct Formatter {
            std::string operator()(const T& arg) {
                std::ostringstream os;
                os<<arg;
                return os.str();
            }
        };

        std::string format_helper(std::string& sofar, const std::string& fmt) {
            return sofar + fmt;
        }
        template<typename T, typename... Args>
        std::string format_helper(std::string& sofar, const std::string& fmt,
                                  const T& arg0, Args&&... args) {
            auto end = fmt.find("{}", 0);
            auto arg = end == std::string::npos ? "" : Formatter<T>()(arg0);
            auto rest = end == std::string::npos ? "" : fmt.substr(end+2);
            sofar += fmt.substr(0, end) + arg;
            return format_helper(sofar, rest, std::forward<Args>(args)...);
        }
        /// (Very basic) string formatting function
        ///
        /// Use {} as a placeholder for arguments
        ///
        /// e.g. format("Hello {}!", "World");
        template<typename... Args>
        std::string format(const std::string& fmt, Args&&... args) {
            std::string ret;
            return format_helper(ret, fmt, std::forward<Args>(args)...);
        }
    }
}

#endif // NTA_FORMATTER_H_INCLUDED