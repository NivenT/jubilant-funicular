#ifndef NTA_LOGGER_H_INCLUDED
#define NTA_LOGGER_H_INCLUDED

#include <fstream>

#include "nta/Errors.h"
#include "nta/format.h"

namespace nta {
    /// stores program information in internal and external logs
    class Logger {
    private:
        /// the file that keeps the external log
        static std::ofstream m_logFile;
        /// Optional secondary stream to log data to as well
        static std::ostream* m_secondLog;
        static size_t m_tabs;
        static const size_t TAB_SIZE;
    public:
        /// creates the log
        static void createLog();
        /// Sets a secondary stream for logging
        static void useSecondLog(std::ostream& stream) { m_secondLog = std::addressof(stream); }
        /// Stop using a secondary stream
        static void unuseSecondLog() { m_secondLog = nullptr; }
        /// writes an entry in the log
        static void writeToLog(crstring entry);
        template<typename... Args>
        static void writeToLog(crstring entry, Args&&... args) { 
            writeToLog(utils::format(entry, std::forward<Args>(args)...)); 
        }
        /// writes entry in log and then notifies ErrorManager
        static Error writeErrorToLog(crstring error, ErrorType type = OTHER);
        template<typename... Args>
        static Error writeErrorToLog(ErrorType type, crstring err, Args&&... args) {
            return writeErrorToLog(utils::format(err, std::forward<Args>(args)...), type);
        }
        /// indents entries
        static void indent(size_t tab_size = TAB_SIZE);
        /// unindents entries
        static void unindent(size_t tab_size = TAB_SIZE);
    };
    /// Simple RAII type for logging entry/exit of scope
    class ScopeLog {
    private:
        const std::string on_exit;
    public:
        ScopeLog(crstring entry, crstring exit);
        ~ScopeLog();
    };
}

#endif // NTA_LOGGER_H_INCLUDED
