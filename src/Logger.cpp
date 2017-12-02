#include <SDL2/SDL.h>

#include "Logger.h"
#include "InputManager.h"

namespace nta {
    std::ofstream   Logger::m_logFile;
    void Logger::createLog() {
        m_logFile.open("Log.log");
        m_logFile<<"";
        writeToLog("Log created");
    }
    void Logger::writeToLog(crstring entry) {
        std::stringstream logEntry;
        logEntry<<SDL_GetTicks()/1000.<<" seconds: "<<entry;
        m_logFile<<logEntry.str()<<std::endl<<std::endl;
    }
    void Logger::writeErrorToLog(crstring error) {
        writeToLog("********** ERROR **********");
        writeToLog(error);
        writeToLog("********** ERROR **********");
        exit(0xbad);
    }
}
