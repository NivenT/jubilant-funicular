#ifndef NTA_STATICVECTOR_H_INCLUDED
#define NTA_STATICVECTOR_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// A (fixed capacity) vector with data stored on the stack instead of the heap.
        ///
        /// Currently does no error checking, so use carefully.
        template<typename T, std::size_t Cap>
        class StaticVector {
        private:
            T m_data[Cap];
            std::size_t m_size;
        public:
            StaticVector() : m_size(0) {}
            StaticVector(const std::initializer_list<T>& data) : m_size(0) {
                for (const T& elem : data) push_back(elem);
            }

            constexpr std::size_t capacity() const { return Cap; }
            std::size_t size() const { return m_size; }
            bool is_empty() const { return m_size == 0; }
            bool empty() const { return is_empty(); }

            T& front() { return m_data[0]; }
            T& back() { return m_data[m_size-1]; }
            T* data() { return &m_data[0]; }

            T& operator[](std::size_t idx) { return m_data[idx]; }
            T& at(std::size_t idx) { return m_data[idx]; }

            void clear() { m_size = 0; }
            void pop_back() { m_size--; }
            void resize(std::size_t size) { m_size = size; }

            void push_back(const T& elem) { m_data[m_size++] = elem; }
            template<typename... Args>
            void emplace_back(Args&&... args) {
                m_data[m_size++] = T(std::forward<Args>(args)...);
            }

            T* begin() { return &m_data[0]; }
            T* end() { return &m_data[m_size]; }
            const T* cbegin() const { return &m_data[0]; }
            const T* cend() const { return &m_data[m_size]; }
        };
    }
}

#endif // NTA_STATICVECTOR_H_INCLUDED