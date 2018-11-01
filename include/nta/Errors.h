#ifndef NTA_ERRORS_H_INCLUDED
#define NTA_ERRORS_H_INCLUDED

#include <functional>

#include "nta/MyEngine.h"

/// \todo Add Option type
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
        UNWRAP_WRONG_RESULT_VARIANT,
        OTHER
    };
    /// Something went wrong
    struct Error {
        Error(crstring desc = "", ErrorType t = OTHER) : description(desc), type(t) {
        }
        Error(const Error& other) {
            description = other.description;
            type = other.type;
            prev = other.prev;
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

    // I wonder how much of my C++ code is influenced by Rust
    /// Used for returning data when an error could potentially occur
    /// \todo Use Result type everywhere it needs to be used
    template<typename T>
    class Result {
    private:
        /// Private constructor (use new_ok or new_err)
        Result() {}

        /// The data or error held by this Result
        union {
            T data;
            Error err;
        };
        /// Whether or not an error occured
        bool is_err_variant;
    public:
        /// Public copy constructor
        Result(const Result& other) {
            is_err_variant = other.is_err_variant;
            if (is_err_variant) {
                new(&err) Error(other.err);
            } else {
                new(&data) T(other.data);
            }
        }
        /// Public destructor
        ~Result() {}
        /// Create new Result holding normal data
        ///
        /// This takes ownership of the data passed in
        static Result new_ok(const T& data) {
            Result ret;
            new(&ret.data) T((data));
            ret.is_err_variant = false;
            return ret;
        }
        /// Create new Result holding an Error
        ///
        /// This takes ownership of the err passed in
        static Result new_err(const Error& err) {
            Result ret;
            new(&ret.err) Error(std::move(err));
            ret.is_err_variant = true;
            return ret;
        }
        /// Is this an Error
        bool is_err() const { return is_err_variant; }
        /// Is this normal data
        bool is_ok() const { return !is_err_variant; }
        /// Get the data (only use if is_ok() returns true)
        T get_data() const {
            if (is_err()) {
                ErrorManager::push_error(Error("Called get_data on err Result",
                                               UNWRAP_WRONG_RESULT_VARIANT));
            }
            return data; 
        }
        /// Same as get_data
        T unwrap() const { return get_data(); }
        /// Get the Error (only use if is_err() returns true)
        Error get_err() const { 
            if (is_ok()) {
                ErrorManager::push_error(Error("Called get_err on ok Result",
                                               UNWRAP_WRONG_RESULT_VARIANT));
            }
            return err; 
        }
        /// Tries getting data, returning a default value if this is an error
        T get_data_or(T optb) const { return is_err_variant ? optb : data; }
        /// Same as get_data_or
        T unwrap_or(T optb) const { return get_data_or(optb); }
        /// Applies a function if no error has occured, else returns the same error
        template<typename S>
        Result<S> map(std::function<S(T)> func) {
            return is_err_variant ? Result<S>::new_err(err) : Result<S>::new_ok(func(data));
        }
        /// \todo (?) Return an Option<Error> instead?
        void map(std::function<void(T)> func) {
            if (!is_err_variant) func(data);
        }
        /// Converts an error variant of Result<T> to an error variant of Result<S>
        template<typename S>
        Result<S> convert_error() {
            if (is_ok()) {
                ErrorManager::push_error(Error("Called convert_error on ok Result",
                                               UNWRAP_WRONG_RESULT_VARIANT));
            }
            return Result<S>::new_err(err);
        }
    };

    /// converts ErrorType enum to string
    std::string get_errortype_string(ErrorType t);
};

#endif // NTA_ERRORS_H_INCLUDED
