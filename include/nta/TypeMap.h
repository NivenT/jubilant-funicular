#ifndef NTA_TYPEMAP_H_INCLUDED
#define NTA_TYPEMAP_H_INCLUDED

#include <unordered_map>
#include <functional>

#include "nta/MyEngine.h"

#define NTA_TYPEINFO_POINTER_BIT    0b0000'0001
#define NTA_TYPEINFO_SMALL_BIT      0b0000'0010
#define NTA_TYPEINFO_REFERENCE_BIT  0b0000'0100

namespace nta {
    namespace utils {
        /// Struct for storing basic information about a type used by TypeMap
        struct TypeInfo {
        private:
            TypeInfo() {}
        public:
            /// This is the only method that should be used for creating a TypeInfo
            ///
            /// The name is subject to change
            template<typename T>
            static const TypeInfo get() {
                TypeInfo ret;
                ret.hash = typeid(T).hash_code();
                ret.flags |= std::is_pointer<T>::value ? NTA_TYPEINFO_POINTER_BIT : 0;
                ret.flags |= sizeof(T) <= sizeof(void*) ? NTA_TYPEINFO_SMALL_BIT : 0;
                ret.flags |= std::is_reference<T>::value ? NTA_TYPEINFO_REFERENCE_BIT : 0;

                bool call_delete = !ret.is_reference() && !ret.is_small();
                ret.destructor = [call_delete](void* o) {
                    using type = typename std::remove_reference<T>::type;
                    if (call_delete) {
                        delete reinterpret_cast<type*>(o);
                    } else {
                        reinterpret_cast<type*>(o)->~type();
                    }
                };
                return ret;
            }
            bool operator==(const TypeInfo& rhs) const {
                return hash == rhs.hash && rhs.is_reference() == is_reference();
            }
            /// Is this a pointer type
            bool is_pointer() const {
                return flags & NTA_TYPEINFO_POINTER_BIT;
            }
            /// Returns false if this type is more bytes than a pointer
            bool is_small() const {
                return flags & NTA_TYPEINFO_SMALL_BIT;
            }
            /// Is this a reference type
            bool is_reference() const {
                return flags & NTA_TYPEINFO_REFERENCE_BIT;
            }

            /// The type's unique hash code
            std::size_t hash;
            /// A bit array of useful information on this type
            unsigned char flags = 0;
            /// The type's destructor
            std::function<void(void*)> destructor;
        };
    }
}

namespace std {
    template<>
    struct hash<nta::utils::TypeInfo> {
        std::size_t operator()(const nta::utils::TypeInfo info) const {
            return info.hash;
        }
    };
}

namespace nta {
    namespace utils {
        /// A map from types to values of the given type
        ///
        /// Note: Only one value of a given type may be stored
        class TypeMap {
        public:
            using iterator = std::unordered_map<TypeInfo, void*>::iterator;
            using const_iterator = std::unordered_map<TypeInfo, void*>::const_iterator;
        private:
            void destroy(const TypeInfo& info);

            std::unordered_map<TypeInfo, void*> m_map; 
        public:
            TypeMap() {}
            ~TypeMap() { clear(); }

            template<typename T>
            bool contains() const { return m_map.find(TypeInfo::get<T>()) != m_map.end(); }
            template<typename T>
            void insert(T data);
            template<typename T>
            typename std::add_lvalue_reference<T>::type get();
            template<typename T>
            void erase();

            bool empty() const { return m_map.empty(); }
            bool is_empty() const { return empty(); }
            std::size_t size() const { return m_map.size(); }
        
            void clear();

            iterator begin() { return m_map.begin(); }
            const_iterator cbegin() { return m_map.cbegin(); }
            iterator end() { return m_map.end(); }
            const_iterator cend() { return m_map.cend(); }

        };
        template<typename T>
        void TypeMap::insert(T data) {
            using type = typename std::remove_reference<T>::type;

            TypeInfo info = TypeInfo::get<T>();
            // info.is_pointer() => info.is_small()
            if (info.is_reference()) {
                m_map[info] = std::addressof(data);
            } else if (info.is_small()) {
                memcpy(&m_map[info], &data, sizeof(T));
            } else {
                m_map[info] = new type(data);
            }
        }
        template<typename T>
        typename std::add_lvalue_reference<T>::type TypeMap::get() {
            using type = typename std::remove_reference<T>::type;

            TypeInfo info = TypeInfo::get<T>();
            if (m_map.find(info) == m_map.end()) assert(false && "Tried getting nonexistent value from a TypeMap");
            
            if (info.is_reference()) {
                return *(type*)m_map[info];
            } else if (info.is_small()) {
                return *(type*)&m_map[info];
            } else {
                return *(type*)m_map[info];
            }
        }
        template<typename T>
        void TypeMap::erase() {
            if (!contains<T>()) return;

            TypeInfo info = TypeInfo::get<T>();
            destroy(info);
            m_map.erase(info);
        }
    }
}

#endif // NTA_TYPEMAP_H_INCLUDED
