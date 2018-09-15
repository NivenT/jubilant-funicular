#ifndef NTA_TYPEMAP_H_INCLUDED
#define NTA_TYPEMAP_H_INCLUDED

#include <unordered_map>
#include <functional>

#include "nta/MyEngine.h"

#define NTA_TYPEINFO_POINTER_BIT    0b0000'0001
#define NTA_TYPEINFO_SMALL_BIT      0b0000'0010
#define NTA_TYPEINFO_TRIV_DEST_BIT  0b0000'0100
#define NTA_TYPEINFO_REFERENCE_BIT  0b0000'1000

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
                ret.flags |= std::is_trivially_destructible<T>::value ? NTA_TYPEINFO_TRIV_DEST_BIT : 0;
                ret.flags |= std::is_reference<T>::value ? NTA_TYPEINFO_REFERENCE_BIT : 0;
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
            /// Is it ok to not call this type's destructor
            bool is_triv_dest() const {
                return flags & NTA_TYPEINFO_TRIV_DEST_BIT;
            }
            /// Is this a reference type
            bool is_reference() const {
                return flags & NTA_TYPEINFO_REFERENCE_BIT;
            }

            /// The types unique hash code
            std::size_t hash;
            /// A bit array of useful information on this type
            unsigned char flags = 0;
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
        /// Note: This class does not take ownership of any stored pointers
        ///       It is up to the client to delete them if necessary.
        /// Warning: For (most) non-pointer types, this class allocates memory on
        ///          on the heap when insert is called. Because of type erasure
        ///          the inserted value's destructor will never be called, so this
        ///          map cannot store (most) types with non-trivial destructors.
        class TypeMap {
        public:
            using iterator = std::unordered_map<TypeInfo, void*>::iterator;
            using const_iterator = std::unordered_map<TypeInfo, void*>::const_iterator;
        private:
            std::unordered_map<TypeInfo, void*> m_map; 
        public:
            TypeMap() {}
            ~TypeMap() { clear(); }

            template<typename T>
            bool contains() const { return m_map.find(TypeInfo::get<T>()) != m_map.end(); }
            template<typename T>
            bool insert(T data);
            template<typename T>
            typename std::add_lvalue_reference<T>::type get();
            template<typename T>
            void erase() { m_map.erase(TypeInfo::get<T>()); }

            bool empty() const { return m_map.empty(); }
            bool is_empty() const { return empty(); }
            std::size_t size() const { return m_map.size(); }
        
            void clear();

            iterator begin() { return m_map.begin(); }
            const_iterator cbegin() { return m_map.cbegin(); }
            iterator end() { return m_map.end(); }
            const_iterator cend() { return m_map.cend(); }

        };
        /// Returns true if data was successfully inserted
        template<typename T>
        bool TypeMap::insert(T data) {
            TypeInfo info = TypeInfo::get<T>();
            // info.is_pointer() => info.is_small()
            if (info.is_reference()) {
                m_map[info] = std::addressof(data);
            } else if (info.is_small()) {
                memcpy(&m_map[info], &data, sizeof(T));
            } else if (info.is_triv_dest()) {
                m_map[info] = malloc(sizeof(T));
                memcpy(m_map[info], &data, sizeof(T));
            } else {
                //assert(false && "Tried inserting a (large) type with non-trivial destructor");
                return false;
            }
            return true;
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
    }
}

#endif // NTA_TYPEMAP_H_INCLUDED
