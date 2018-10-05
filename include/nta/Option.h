#ifndef NTA_OPTION_H_INCLUDED
#define NTA_OPTION_H_INCLUDED

#include <functional>

#include "nta/MyEngine.h"
#include "nta/Wrapper.h"

namespace nta {
    namespace utils {
        /// A class that optionally holds some data
        ///
        /// Used to signal that a function sometimes returns nothing
        template<typename T>
        class Option {
        private:
            using type = typename std::remove_reference<T>::type;

            /// Private constructor (use some or none instead)
            Option() : m_data(*(type*)(new char(0))), m_some(false) {}
            Option(const T& d) : m_data(d), m_some(true) {}

            T m_data;
            bool m_some;
        public:
            Option(const Option&) = default;
            ~Option() { m_some = false; }
            /// Creates an Option holding some data
            static Option some(const T& data);
            /// Creates a None variant Option
            static Option none();
            /// Does this hold some data
            bool is_some() const { return m_some; }
            /// Does this hold nothing?
            bool is_none() const { return !m_some; }
            /// Retreive the underlying data
            ///
            /// You should always call is_some/is_none beforehand
            T get() const;
            /// unwrap and get are the same thing
            T unwrap() const { return get(); }
            /// Return the data held by this Option or optb if it's None
            T get_or(const T& optb) const { return m_some ? m_data : optb; }
            T unwrap_or(const T& optb) const { return get_or(optb); }
            /// Returns an Option holding the result of applying func to data
            template<typename S>
            Option<S> map(std::function<S(T)> func);
            template<typename S>
            S map_or(std::function<S(T)> func, const S& def);
            void map(std::function<void(T)> func);
        };
        template<typename T>
        Option<T> Option<T>::some(const T& data) {
            return Option<T>(data);
        }
        template<typename T>
        Option<T> Option<T>::none() {
            return Option<T>();
        }
        template<typename T>
        T Option<T>::get() const {
            /// \todo Push an error to ErrorManager instead
            if (!m_some) {
                assert(false && "Tried getting data from a none Option");
            }
            return m_data;
        }
        template<typename T> template<typename S>
        Option<S> Option<T>::map(std::function<S(T)> func) {
            return m_some ? Option<S>::some(func(m_data)) : Option<S>::none();
        }
        template<typename T> template<typename S>
        S Option<T>::map_or(std::function<S(T)> func, const S& def) {
            return m_some ? func(m_data) : def;
        }
        template<typename T>
        void Option<T>::map(std::function<void(T)> func) {
            if (m_some) func(m_data);
        }
        template<typename T, typename std::enable_if_t<check::LShiftExists<std::ostream, T>::value>* = nullptr>
        std::ostream& operator<<(std::ostream& lhs, const Option<T>& rhs) {
            if (rhs.is_some()) {
                return lhs<<"Some("<<rhs.unwrap()<<")";
            } else {
                return lhs<<"None";
            }
        }
        /// (Untested) Specialization of Option<T>. Every Option<void> is None
        template<>
        class Option<void> {
        private:
            Option();
        public:
            static Option some() { return Option(); }
            static Option none() { return Option(); }
            bool is_some() const { return false; }
            bool is_none() const { return true; }
            void get() const {}
            void unwrap() const {}
            void get_or() const {}
            void unwrap_or() const {}
            /// This may not be the right choice, but calling map returns
            /// an option holding some data, and not a none variant.
            template<typename S>
            Option<S> map(std::function<S(void)> func) { return Option<S>::some(func()); }
        };
    }
};

#endif // NTA_OPTION_H_INCLUDED
