#include <SDL2/SDL.h>

#include "nta/Logger.h"

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
        m_logFile<<lock_stream<<logEntry.str()<<std::endl<<std::endl<<unlock_stream;
    }
    /// \todo Make sure all the code that previously assumed this function always
    ///       exits still behaves sensibly
    Error Logger::writeErrorToLog(crstring error, ErrorType type) {
        const std::string err_indicator = 
            "********** ERROR (" + get_errortype_string(type) + ") **********";

        writeToLog(err_indicator);
        writeToLog(error);
        writeToLog(err_indicator);

        Error err = Error(error, type);
        ErrorManager::push_error(err);
        return err;
    }
    void Logger::indent(size_t tab_size) {
        m_tabs += tab_size;
    }
    void Logger::unindent(size_t tab_size) {
        if (m_tabs >= tab_size) m_tabs -= tab_size;
    }
}
