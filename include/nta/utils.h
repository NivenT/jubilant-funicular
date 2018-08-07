#ifndef NTA_UTILS_H_INCLUDED
#define NTA_UTILS_H_INCLUDED

#include "nta/MyEngine.h"

#include <string>
#include <vector>

namespace nta {
	namespace utils {
		extern bool starts_with(crstring& str, crstring& prefix);
		extern bool ends_with(crstring& str, crstring& suffix);
		extern std::string replace_all(crstring& str, crstring o, crstring n);
		extern std::string replace_all(crstring& str, const std::vector<std::vector<std::string>> os, const std::vector<std::string> ns);
		extern std::string trim(crstring& str);
		extern std::vector<std::string> split(crstring str, char delim);
		/// Rotates a point (about the origin) by the given angle
    	extern glm::vec2 rotate(crvec2 pt, float angle);

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
	}
}

#endif // NTA_UTILS_H_INCLUDED