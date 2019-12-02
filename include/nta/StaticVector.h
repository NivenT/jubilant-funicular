#ifndef NTA_STATICVECTOR_H_INCLUDED
#define NTA_STATICVECTOR_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// A (fixed capacity) vector with data stored on the stack instead of the heap.
        ///
        /// Currently does no error checking, so use carefully.
        /// \todo Add tests
        template<typename T, std::size_t Cap>
        class StaticVector {
        private:
            typename std::aligned_storage_t<sizeof(T), alignof(T)> m_data[Cap];
            std::size_t m_size;
        public:
            StaticVector() : m_size(0) {}
            StaticVector(const std::initializer_list<T>& data) : m_size(0) {
                for (const T& elem : data) push_back(elem);
            }
            ~StaticVector() { clear(); }

            constexpr std::size_t capacity() const { return Cap; }
            std::size_t size() const { return m_size; }
            bool is_empty() const { return m_size == 0; }
            bool empty() const { return is_empty(); }

            T& front() { return at(0); }
            T& back() { return at(m_size-1); }
            T* data() { return begin(); }

            T& operator[](std::size_t idx) { return at(idx); }
            T& at(std::size_t idx) { return *(data() + idx); }

            void clear() { while (m_size > 0) pop_back(); }
            void pop_back() { at(--m_size).~T(); }
            // When you value using only one line over efficiency
            void resize(std::size_t size) { 
                while (size < (m_size = std::max(m_size, size))) pop_back();
            }

            void push_back(const T& elem) { new(&m_data[m_size++]) T(elem); }
            template<typename... Args>
            void emplace_back(Args&&... args) {
                new(&m_data[m_size++]) T(std::forward<Args>(args)...);
            }

            T* begin() { return reinterpret_cast<T*>(std::launder(&m_data[0])); }
            T* end() { return begin() + m_size; }
            const T* cbegin() const { 
                return reinterpret_cast<const T*>(std::launder(&m_data[0])); 
            }
            const T* cend() const { return cbegin() + m_size; }
        };
    }
}

#endif // NTA_STATICVECTOR_H_INCLUDED