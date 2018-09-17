#include "nta/TypeMap.h"

namespace nta {
    namespace utils {
        void* TypeMap::find(const TypeInfo& info) const { 
            auto it = m_map.find(info); 
            return it == m_map.end() ? nullptr : it->second; 
        }
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
        void TypeMap::erase(const TypeInfo& info) {
            if (m_map.find(info) == m_map.end()) return;
            destroy(info);
            m_map.erase(info);
        }
        void TypeMap::clear() {
            for (auto& pair : m_map) {
                destroy(pair.first);
            }
            m_map.clear();
        }
    }
}