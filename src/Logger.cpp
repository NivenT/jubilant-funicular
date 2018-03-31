#include <SDL2/SDL.h>

#include "nta/Logger.h"
#include "nta/CallbackManager.h"

namespace nta {
    std::ofstream   Logger::m_logFile;
    size_t          Logger::m_tabs = 0;
    const size_t    Logger::TAB_SIZE = 4;
    void Logger::createLog() {
        m_logFile.open("Log.log");
        m_logFile<<"";
        writeToLog("Log created");
    }
    void Logger::writeToLog(crstring entry) {
        std::string tabs(m_tabs, ' ');

        std::stringstream logEntry;
        logEntry<<SDL_GetTicks()/1000.<<" seconds: "<<tabs<<entry;
        m_logFile<<logEntry.str()<<std::endl<<std::endl;
    }
    void Logger::writeErrorToLog(crstring error) {
        writeToLog("********** ERROR **********");
        writeToLog(error);
        writeToLog("********** ERROR **********");
        m_tabs = 0;
        cleanup();
        exit(0xbad);
    }
    void Logger::indent(size_t tab_size) {
        m_tabs += tab_size;
    }
    void Logger::unindent(size_t tab_size) {
        if (m_tabs >= tab_size) m_tabs -= tab_size;
    }
}
