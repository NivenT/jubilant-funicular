#ifndef NTA_PATH_H_INCLUDED
#define NTA_PATH_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
	namespace utils {
		class Path;
		extern std::ostream& operator<<(std::ostream& out, const Path& path);
		/// A Path in a file system
		class Path {
		private:
			std::string m_path;
		public:
			Path();
			Path(crstring path) : m_path(path) {}

			Path& operator=(crstring p) { m_path = p; return *this; }

			const std::string& to_string() { return m_path; }

			friend std::ostream& operator<<(std::ostream& out, const Path& path);
		};
	}
}

#endif // NTA_PATH_H_INCLUDED