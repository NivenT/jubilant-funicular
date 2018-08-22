#include <fstream>

#ifdef NTA_UNIX
	#include <sys/stat.h>
	#include <unistd.h>
#elif NTA_WINDOWS
	#include <direct.h>
#endif

#include "nta/Path.h"
#include "nta/utils.h"

namespace nta {
	namespace utils {
		Path::Path(crstring path) {
			m_path = replace_all(path, "\\", "/");
		}
		Path Path::cwd() {
			char buf[FILENAME_MAX];
			#ifdef NTA_UNIX
				getcwd(buf, FILENAME_MAX);
			#elif NTA_WINDOWS
				_getcwd(buf, FILENAME_MAX);
			#endif
			return Path(buf);
		}
		Path& Path::resolve() {
			if (is_empty()) return *this;
			if (is_relative()) *this = Path::cwd() + *this;
			auto parts = split(m_path, '/');
			for (int i = parts.size()-1; i >= 0; i--) {
				auto& part = parts[i];
				if ((part.empty() || part == ".") && i != 0) {
					parts.erase(parts.begin() + i);
				} else if (part == ".." && i >= 1) {
					parts.erase(parts.begin() + i);
					parts.erase(parts.begin() + i - 1);
				}
			}
			m_path = "";
			for (auto& part : parts) m_path += part + "/";
			return *this;
		}
		/// \todo Make work for relative paths
		Path Path::parent() const {
			Path ret(m_path);
			do {
				int pos = ret.m_path.rfind("/", ends_with(m_path, "/") ? m_path.size()-2 : std::string::npos);
				ret.m_path = ret.m_path.substr(0, pos);
			} while (ret.m_path.back() == '/');
			return ret == *this ? "." : ret + "/";
		}
		std::string Path::file_name(bool extension) const {
			int pos = m_path.rfind('/');
			std::string end = m_path.substr(pos+1);
			if (extension) return end;
			pos = end.rfind('.');
			return end.substr(0, pos);
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
			Path p2(p);
			return ends_with(m_path, "/") ? Path(m_path + p2.m_path) : 
											Path(m_path + "/" + p2.m_path);
		}
		Path::iterator Path::begin() const {
			iterator ret(this);
			ret.set_begin(SetBeginEndKey());
			return ret;
		}
		Path::iterator Path::end() const {
			iterator ret(this);
			ret.set_end(SetBeginEndKey());
			return ret;
		}

		std::ostream& operator<<(std::ostream& out, const Path& path) {
			return out<<path.m_path;
		}
	}
}