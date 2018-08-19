#include <fstream>

#ifdef NTA_UNIX
	#include <sys/stat.h>
#elif NTA_WINDOWS
	#include <Windows.h>
#endif

#include "nta/Path.h"
#include "nta/utils.h"

namespace nta {
	namespace utils {
		Path::Path() {
		}
		/// \todo Make word for relative paths
		Path Path::parent() const {
			Path ret(m_path);
			do {
				int pos = ret.m_path.rfind("/");
				ret.m_path = ret.m_path.substr(0, pos);
			} while (ret.m_path.back() == '/');
			return ret + "/";
		}
		bool Path::exists() const {
			return std::ifstream(m_path).good();
		}
		bool Path::is_file() const {
			// Technically wrong because of symlinks and whatnot
			return exists() && !is_directory();
		}
		bool Path::is_directory() const {
			if (!exists()) return false;
			#ifdef NTA_UNIX
				struct stat s;
				if (stat(m_path.c_str(), &s) == 0) {
					return s.st_mode & S_IFDIR;
				} else {
					/// \todo Error
					return false;
				}
			#elif NTA_WINDOWS
				return GetFileAttributes(m_path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
			#endif
		}
		bool Path::is_absolute() const {
			if (m_path.empty()) return false;
			int pos = m_path.find('/');
			std::string root = m_path.substr(0, pos);
			return root.empty() || (root.size() == 2 && root[1] == ':');
		}
		bool Path::operator==(const char* p) const {
			return trim(m_path, "/\\") == trim(p, "/\\");
		}
		Path Path::operator+(const char* p) const {
			return ends_with(m_path, "/") ? Path(m_path + p) : Path(m_path + "/" + p);
		}

		std::ostream& operator<<(std::ostream& out, const Path& path) {
			return out<<path.m_path;
		}
	}
}