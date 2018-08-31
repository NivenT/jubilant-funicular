#ifndef NTA_MESSAGE_H_INCLUDED
#define NTA_MESSAGE_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
	/// Something someone needs to be notified about
    struct Message {
        Message() : type(0), data(nullptr) {}
        Message(uint64_t type) : type(type), data(nullptr) {}
        Message(uint64_t type, const void* data) : type(type), data(data) {}

        // This, I think, is mostly useless in applications
        uint64_t operator&(uint64_t rhs) const { return type & rhs; }
        bool operator==(uint64_t rhs) const { return type == rhs; }

        /// bit array (or bit mask) specifying type of message 
        ///
        /// The semantic meaning of this variable is user-defined
        uint64_t type;
        /// arbitrary data associated with this Message
        const void* data;
    };
}

#endif // NTA_MESSAGE_H_INCLUDED
