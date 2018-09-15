#include "nta/TypeMap.h"

namespace nta {
    namespace utils {
        void TypeMap::clear() {
            for (auto& pair : m_map) {
                if (!pair.first.is_small()) {
                    free(pair.second);
                }
            }
            m_map.clear();
        }
    }
}