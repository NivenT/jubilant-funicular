#ifndef NTA_PATH_H_INCLUDED
#define NTA_PATH_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
	namespace utils {
		/// A Path in a file system
		class Path {
		private:
			std::string m_path;
		public:
			Path();
			Path(crstring path) : m_path(path) {}
		};
	}
}

#endif // NTA_PATH_H_INCLUDED