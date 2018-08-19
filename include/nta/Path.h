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
			Path(const char* p) : m_path(p) {}

			Path parent() const;

			bool exists() const;
			bool is_file() const;
			bool is_directory() const;
			bool is_relative() const { return !is_absolute(); }
			bool is_absolute() const;

			Path& operator=(const char* p) { m_path = p; return *this; }
			Path& operator=(crstring p) { operator=(p.c_str()); }
			bool operator==(const char* p) const;
			bool operator==(const Path& p) const { return *this == p.m_path; }
			bool operator==(crstring p) const { return operator==(p.c_str()); }

			Path operator+(const char* p) const;
			Path& operator+=(const char* p) { return *this = *this + p; }
			Path operator+(crstring p) const { return operator+(p.c_str()); }
			Path& operator+=(crstring p) { return operator+=(p.c_str()); }
			Path operator+(const Path& p) const { return operator+(p.m_path); }
			Path& operator+=(const Path& p) { return operator+=(p.m_path); }

			const std::string& to_string() { return m_path; }

			friend std::ostream& operator<<(std::ostream& out, const Path& path);
		};
	}
}

#endif // NTA_PATH_H_INCLUDED