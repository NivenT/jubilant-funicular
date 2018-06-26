#include <iostream>

#include "nta/Errors.h"

namespace nta {
	const Error*  ErrorManager::m_errors = nullptr;
	ErrorCallback ErrorManager::m_periodic_callback = [](const Error* _) {};
	ErrorCallback ErrorManager::m_push_callback = [](const Error* err) {
		if (err) {
			cleanup();
        	std::cout<<"An error occured. Check Log.log for details"<<std::endl;
        	exit(0xbad);
		}
	};

	void ErrorManager::handle_errors() {
		if (m_periodic_callback) {
			m_periodic_callback(m_errors);
			clear_errors();
		}
	}
	void ErrorManager::set_periodic_callback(const ErrorCallback& callback) {
		m_periodic_callback = callback;
	}
	void ErrorManager::set_push_callback(const ErrorCallback& callback) {
		m_push_callback = callback;
	}
	void ErrorManager::push_error(const Error& err) {
		// Want to own the pointer in m_errors
		Error* ptr = new Error(err);
		if (m_push_callback) m_push_callback(ptr);
		ptr->prev = m_errors;
		m_errors = ptr;
	}
	const Error* ErrorManager::pop_error() {
		const Error* peek = m_errors;
		m_errors = m_errors ? m_errors->prev : nullptr;
		return peek;
	}
	const Error* ErrorManager::peek_error() {
		return m_errors;
	}
	void ErrorManager::clear_errors() {
		delete m_errors;
		m_errors = nullptr;
	}

	std::string get_errortype_string(ErrorType t) {
        switch(t) {
        case MISSING_RESOURCE:
            return "Missing Resource";
        case INVALID_VALUE:
            return "Invalid value";
        case GL_FAILURE:
            return "GL Failure";
        case SDL_FAILURE:
            return "SDL Failure";
        case DEVIL_FAILURE:
            return "DevIL Failure";
        case IMPOSSIBLE_BEHAVIOR:
        	return "Impossible Behavior";
        case UNWRAP_WRONG_RESULT_VARIANT:
        	return "Unwrapped wrong result variant";
        case OTHER:
            return "Other";
        }
    }
}