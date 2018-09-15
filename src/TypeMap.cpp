#include "nta/TypeMap.h"

namespace nta {
    namespace utils {
        void TypeMap::destroy(const TypeInfo& info) {
            if (info.is_reference()) {
                info.destructor(m_map[info]);
            } else if (info.is_small()) {
                info.destructor(&m_map[info]);
            } else {
                // calls delete
                info.destructor(m_map[info]);
            }
        }
        void TypeMap::clear() {
            for (auto& pair : m_map) {
                destroy(pair.first);
            }
            m_map.clear();
        }
    }
}