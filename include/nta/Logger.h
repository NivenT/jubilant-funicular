#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <fstream>

#include "MyEngine.h"

namespace nta {
    ///stores program information in internal and external logs
    class Logger {
    private:
        ///the file that keeps the external log
        static std::ofstream m_logFile;
    public:
        ///creates the log
        static void createLog();
        ///writes an entry in the log
        static void writeToLog(crstring entry);
        ///writes entry in log and then exits program
        static void writeErrorToLog(crstring error);
    };
}

#endif // LOGGER_H_INCLUDED
