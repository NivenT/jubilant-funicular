#include <algorithm>
#include <unordered_set>

#include "nta/Path.h"

namespace nta {
	namespace utils {
		Path::Path() {
		}

		std::ostream operator<<(std::ostream& out, const Path& path) {
			out<<path.m_path;
		}
	}
}