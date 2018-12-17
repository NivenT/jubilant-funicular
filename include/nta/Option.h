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
            /// Only exists to avoid a warning when using Option<T&>
            struct Nop { Nop(const type&& _) {} };

            using storage_type = typename std::conditional<std::is_reference<T>::value,
                                                           void*, T>::type;
            using placement_type = typename std::conditional<std::is_reference<T>::value,
                                                             Nop, type>::type;

            /// Private constructor (use some or none instead)
            Option(const T& d) : m_some(true) { 
                if (std::is_reference<T>::value) {
                    // Maybe I should just memcpy instead
                    new(&m_data) const void*(std::addressof(d));
                } else {
                    new(&m_data) placement_type(std::move(d));
                }
            }
            Option() : m_some(false) {}

            typename std::aligned_storage_t<sizeof(storage_type), alignof(storage_type)> m_data;
            bool m_some;
        public:
            Option(const Option&) = default;
            ~Option() { m_some = false; }
            /// Creates an Option holding some data
            static Option some(const T& data);
            /// Creates a None variant Option
            static Option none();
            /// Same as is_some
            operator bool() const { return m_some; }
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
            T get_or(const T& optb) { return m_some ? get() : optb; }
            T unwrap_or(const T& optb) { return get_or(optb); }
            /// Turns this into None variant, calling destructor if necessary
            void destroy();
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
            } else if (std::is_reference<T>::value) {
                return (T)**reinterpret_cast<const type * const *>(&m_data);
            } else {
                return (T)*reinterpret_cast<const type*>(&m_data);
            }
        }
        template<typename T>
        void Option<T>::destroy() {
            if (m_some) {
                reinterpret_cast<type*>(&m_data)->~type();
            }
            m_some = false;
        }
        template<typename T> template<typename S>
        Option<S> Option<T>::map(std::function<S(T)> func) {
            return m_some ? Option<S>::some(func(get())) : Option<S>::none();
        }
        template<typename T> template<typename S>
        S Option<T>::map_or(std::function<S(T)> func, const S& def) {
            return m_some ? func(get()) : def;
        }
        template<typename T>
        void Option<T>::map(std::function<void(T)> func) {
            if (m_some) func(get());
        }
        template<typename T, typename std::enable_if_t<check::LShiftExists<std::ostream, T>::value>* = nullptr>
        std::ostream& operator<<(std::ostream& lhs, const Option<T>& rhs) {
            if (rhs.is_some()) {
                return lhs<<"Some("<<rhs.unwrap()<<")";
            } else {
                return lhs<<"None";
            }
        }
    }
};

#endif // NTA_OPTION_H_INCLUDED
