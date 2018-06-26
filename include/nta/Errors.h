#ifndef NTA_ERRORS_H_INCLUDED
#define NTA_ERRORS_H_INCLUDED

#include <functional>

#include "MyEngine.h"

namespace nta {
	struct Error;
	typedef std::function<void(const Error*)> ErrorCallback;
	/// Types of Error
	enum ErrorType {
		MISSING_RESOURCE,
		INVALID_VALUE,
		GL_FAILURE,
		SDL_FAILURE,
		DEVIL_FAILURE,
		IMPOSSIBLE_BEHAVIOR,
		OTHER
	};
	/// Something went wrong
    struct Error {
    	Error(crstring desc = "", ErrorType t = OTHER) : description(desc), type(t) {
    	}
    	~Error() {
    		if (prev) delete prev;
    	}
    	/// A description of what when wrong
    	std::string description;
    	/// The type of error that occured
    	ErrorType type;
    	/// The most recent unhandled error before this one
    	const Error* prev = nullptr;
    };
    /// Stores and handles Errors
    /// \todo There should be one of each "Manager" class type per ScreenManager
    ///       or one per Window (i.e. they should not be static)
    class ErrorManager {
    private:
    	/// A linked list of unhandled errors
    	static const Error* m_errors;
    	/// The function used to handle errors at the end of each frame
    	static ErrorCallback m_periodic_callback;
    	/// The function used to handle errors as they occur
    	static ErrorCallback m_push_callback;
    public:
    	/// Handles all outstanding errors
    	static void handle_errors();
    	/// Sets the periodic callback function
    	static void set_periodic_callback(const ErrorCallback& callback);
    	/// Sets the push callback function
    	static void set_push_callback(const ErrorCallback& callback);
    	/// Adds and error to the front of the list
    	static void push_error(const Error& err);
    	/// Removes (and returns) the error from the front of the list
    	static const Error* pop_error();
    	/// Returns the error at the front of the list
    	static const Error* peek_error();
    	/// Deletes all errors (without handling any of them)
    	static void clear_errors();
    };

    /// converts ErrorType enum to string
    std::string get_errortype_string(ErrorType t);
};

#endif // NTA_ERRORS_H_INCLUDED
