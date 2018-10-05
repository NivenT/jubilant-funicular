#ifndef NTA_WRAPPER_H_INCLUDED
#define NTA_WRAPPER_H_INCLUDED

#include <utils/wrapper/Wrapper.h>

namespace nta {
    namespace utils {
        /// \todo Reexport everything from ::utils::check
        namespace check {
            typedef ::utils::check::Nop Nop;
            template<typename T, typename U=T>
            using LShiftExists = ::utils::check::LShiftExists<T, U>;
        }
        template<typename T, typename TAG>
        using Wrapper = ::utils::Wrapper<T, TAG>;
    }
}


#endif // NTA_WRAPPER_H_INCLUDED