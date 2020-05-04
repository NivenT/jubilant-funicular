#ifndef NTA_TYPETRAITS_H_INCLUDED
#define NTA_TYPETRAITS_H_INCLUDED

#include <type_traits>

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        template<typename T, std::size_t = sizeof(T)>
        std::true_type is_defined_impl(T*);
        std::false_type is_defined_impl(...);
        template<typename T>
        using is_defined = decltype(is_defined_impl(std::declval<T*>()));

        template<typename T, typename = std::void_t<>>
        inline constexpr bool can_cout = false;
        template<typename T>
        inline constexpr bool can_cout<T, std::void_t<decltype(
            std::declval<std::ostream&>()<<std::declval<const T&>()
        )>> = true;

        template<typename T, typename  = T, typename = std::void_t<>>
        inline constexpr bool can_check_equality = false;
        template<typename T, typename U>
        inline constexpr bool can_check_equality<T, U, std::void_t<decltype(
            std::declval<T>() == std::declval<U>()
        )>> = true;
    }
}

#endif // NTA_TYPETRAITS_H_INCLUDED