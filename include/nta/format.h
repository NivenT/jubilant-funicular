#ifndef NTA_FORMATTER_H_INCLUDED
#define NTA_FORMATTER_H_INCLUDED

#include <string>
#include <sstream>
#include <iostream>

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// (Very basic) string formatting function
        ///
        /// Use {} as a placeholder for arguments
        ///
        /// e.g. format("Hello {}!", "World");
        template<typename... Args>
        std::string format(const std::string& fmt, Args&&... args);

        /// Specialize this struct to use custom types with format
        template<typename T>
        struct Formatter {
            std::string operator()(const T& arg) {
                std::ostringstream os;
                os<<arg;
                return os.str();
            }
        };
        // Should I not have these here in jubliant-funicular?
        template<>
        struct Formatter<bool> {
            std::string operator()(const bool& arg) {
                return arg ? "true" : "false";
            }
        };
        template<>
        struct Formatter<glm::vec2> {
            std::string operator()(const glm::vec2& arg) {
                return format("({}, {})", arg.x, arg.y);
            }
        };
        template<>
        struct Formatter<glm::vec3> {
            std::string operator()(const glm::vec3& arg) {
                return format("({}, {}, {})", arg.x, arg.y, arg.z);
            }
        };
        template<>
        struct Formatter<glm::vec4> {
            std::string operator()(const glm::vec4& arg) {
                return format("({}, {}, {}, {})", arg.x, arg.y, arg.z, arg.w);
            }
        };
        template<>
        struct Formatter<glm::ivec2> {
            std::string operator()(const glm::ivec2& arg) {
                return format("({}, {})", arg.x, arg.y);
            }
        };
        template<>
        struct Formatter<glm::ivec3> {
            std::string operator()(const glm::ivec3& arg) {
                return format("({}, {}, {})", arg.x, arg.y, arg.z);
            }
        };
        template<>
        struct Formatter<glm::ivec4> {
            std::string operator()(const glm::ivec4& arg) {
                return format("({}, {}, {}, {})", arg.x, arg.y, arg.z, arg.w);
            }
        };

        extern std::string format_helper(std::string& sofar, const std::string& fmt);
        template<typename T, typename... Args>
        std::string format_helper(std::string& sofar, const std::string& fmt,
                                  const T& arg0, Args&&... args) {
            auto end = fmt.find("{}", 0);
            auto arg = end == std::string::npos ? "" : Formatter<T>()(arg0);
            auto rest = end == std::string::npos ? "" : fmt.substr(end+2);
            sofar += fmt.substr(0, end) + arg;
            return format_helper(sofar, rest, std::forward<Args>(args)...);
        }
        template<typename... Args>
        std::string format(const std::string& fmt, Args&&... args) {
            std::string ret;
            return format_helper(ret, fmt, std::forward<Args>(args)...);
        }
        template<typename... Args>
        std::ostream& print(const std::string& fmt, Args&&... args) {
            return std::cout<<format(fmt, std::forward<Args>(args)...);
        }
        template<typename... Args>
        std::ostream& println(const std::string& fmt, Args&&... args) {
            return std::cout<<format(fmt, std::forward<Args>(args)...)<<std::endl;
        }
    }
}

#endif // NTA_FORMATTER_H_INCLUDED