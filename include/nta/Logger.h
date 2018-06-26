#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <fstream>

#include "Errors.h"

namespace nta {
    /// stores program information in internal and external logs
    class Logger {
    private:
        /// the file that keeps the external log
        static std::ofstream m_logFile;
        static size_t m_tabs;
        static const size_t TAB_SIZE;
    public:
        /// creates the log
        static void createLog();
        /// writes an entry in the log
        static void writeToLog(crstring entry);
        /// writes entry in log and then exits program
        static Error writeErrorToLog(crstring error, ErrorType type = OTHER);
        /// indents entries
        static void indent(size_t tab_size = TAB_SIZE);
        /// unindents entries
        static void unindent(size_t tab_size = TAB_SIZE);
    };
}

#endif // LOGGER_H_INCLUDED
