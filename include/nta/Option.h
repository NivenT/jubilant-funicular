#ifndef NTA_OPTION_H_INCLUDED
#define NTA_OPTION_H_INCLUDED

#include <functional>

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// A class that optionally holds some data
        ///
        /// Used to signal that a function sometimes returns nothing
        template<typename T>
        class Option {
        private:
            using type = typename std::remove_reference<T>::type;

            /// Private constructor (use new_some or none instead)
            Option() : data(*(type*)(0)), some(false) {}
            Option(T d) : data(d), some(true) {}

            T data;
            bool some;
        public:
            ~Option() { some = false; }
            /// Creates an Option holding some data
            static Option new_some(const T& data);
            /// Creates a None variant Option
            static Option none();
            /// Does this hold some data
            bool is_some() const { return some; }
            /// Does this hold nothing?
            bool is_none() const { return !some; }
            /// Retreive the underlying data
            ///
            /// You should always call is_some/is_none beforehand
            T get() const;
            /// unwrap and get are the same thing
            T unwrap() const { return get(); }
            /// Return the data held by this Option or optb if it's None
            T get_or(const T& optb) { return some ? data : optb; }
            T unwrap_or(const T& optb) { return get_or(optb); }
            /// Returns an Option holding the result of applying func to data
            template<typename S>
            Option<S> map(std::function<S(T)> func);
        };
        template<typename T>
        Option<T> Option<T>::new_some(const T& data) {
            return Option<T>(data);
        }
        template<typename T>
        Option<T> Option<T>::none() {
            return Option<T>();
        }
        template<typename T>
        T Option<T>::get() const {
            /// \todo Push an error to ErrorManager instead
            if (!some) {
                assert(false && "Tried getting data from a none Option");
            }
            return data;
        }
        template<typename T>
        template<typename S>
        Option<S> Option<T>::map(std::function<S(T)> func) {
            return some ? new_some(func(data)) : none();
        }
    }
};

#endif // NTA_OPTION_H_INCLUDED
