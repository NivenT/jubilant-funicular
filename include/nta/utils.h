#ifndef NTA_UTILS_H_INCLUDED
#define NTA_UTILS_H_INCLUDED

#include "nta/MyEngine.h"

#include <string>
#include <vector>

namespace nta {
	namespace utils {
		/// Checks if str starts with prefix
		extern bool starts_with(crstring& str, crstring& prefix);
		/// Checks if str ends with suffix
		extern bool ends_with(crstring& str, crstring& suffix);
		/// Replaces all occurences of o in str with n
		extern std::string replace_all(crstring& str, crstring o, crstring n);
		/// Replaces all occurences of any string in the ith element of os with the ith element of ns
		extern std::string replace_all(crstring& str, const std::vector<std::vector<std::string>> os, const std::vector<std::string> ns);
		/// Removes leading and trailing whitespace
		extern std::string trim(crstring& str);
		/// Splits a string into substrings separated by delim
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