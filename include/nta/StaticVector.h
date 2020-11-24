#ifndef NTA_STATICVECTOR_H_INCLUDED
#define NTA_STATICVECTOR_H_INCLUDED

#include <memory>
#include <new>

#include "nta/MyEngine.h"

namespace nta {
    namespace utils {
        /// A (fixed capacity) vector with data stored on the stack instead of the heap.
        ///
        /// Currently does no error checking, so use carefully.
        template<typename T, std::size_t Cap>
        class StaticVector {
        public:
            using value_type = T;
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        private:
            typename std::aligned_storage_t<sizeof(T), alignof(T)> m_data[Cap];
            std::size_t m_size;
        public:
            StaticVector() : m_size(0) {}
            template<std::size_t Cap2>
            StaticVector(const StaticVector<T, Cap2>& other) : m_size(other.size()) {
                std::uninitialized_copy(other.cbegin(), other.cend(), begin());
            }
            template<std::size_t Cap2>
            StaticVector(StaticVector<T, Cap2>&& other) : m_size(other.size()) {
                std::uninitialized_copy(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), begin());
            }
            explicit StaticVector(std::size_t count, const T& value = T()) : m_size(count) {
                std::uninitialized_fill(begin(), end(), value);
            }
            StaticVector(const std::initializer_list<T>& data) : m_size(0) {
                for (const T& elem : data) push_back(elem);
            }
            StaticVector(iterator beg, iterator end) : m_size(0) {
                for (auto& it = beg; it != end; ++it) push_back(*it);
            }
            ~StaticVector() { clear(); }

            constexpr std::size_t capacity() const { return Cap; }
            constexpr std::size_t cap() const { return Cap; }
            constexpr std::size_t max_size() const { return Cap; }

            std::size_t size() const { return m_size; }
            bool is_empty() const { return m_size == 0; }
            bool empty() const { return is_empty(); }
            bool is_full() const { return m_size == Cap; }
            bool full() const { return is_full(); }

            T& front() { return at(0); }
            T& back() { return at(m_size-1); }
            T* data() { return begin(); }

            T& operator[](std::size_t idx) { return at(idx); }
            T& at(std::size_t idx) { return *(data() + idx); }

            const T& operator[](std::size_t idx) const { return at(idx); }
            const T& at(std::size_t idx) const { return *(cbegin() + idx); }

            void clear() { while (m_size > 0) pop_back(); }
            void pop_back() { at(--m_size).~T(); }
            void erase(iterator pos) { remove(pos); }
            void resize(std::size_t size) { 
                while (size < (m_size = std::max(m_size, size))) pop_back();
            }
            void remove(iterator pos) {
                for (auto it = (m_size--, pos->~T(), pos); it != cend(); ++it) *it = *(it+1);
            }

            void push_back(const T& elem) { new(&m_data[m_size++]) T(elem); }
            template<typename... Args>
            void emplace_back(Args&&... args) {
                new(&m_data[m_size++]) T(std::forward<Args>(args)...);
            }
            // When you value using only one line over efficiency/readability
            void insert(iterator pos, const T& elem) {
                for (auto it = (m_size++, rbegin()); it != reverse_iterator(pos) || (*pos = elem, false); ++it) *it = *(it+1);
            }
            template<typename... Args>
            void emplace(iterator pos, Args&&... args) {
                for (auto it = (m_size++, rbegin()); it != reverse_iterator(pos) || (new(std::addressof(*pos)) T(std::forward<Args>(args)...), false); ++it) *it = *(it+1);
            }

            iterator begin() { return reinterpret_cast<T*>(std::launder(&m_data[0])); }
            const_iterator begin() const { return cbegin(); }
            iterator end() { return begin() + m_size; }
            const_iterator end() const { return cend(); }
            const_iterator cbegin() const { 
                return reinterpret_cast<const T*>(std::launder(&m_data[0])); 
            }
            const_iterator cend() const { return cbegin() + m_size; }
            reverse_iterator rbegin() { return reverse_iterator(end()); }
            const_reverse_iterator rbegin() const { return crbegin(); }
            reverse_iterator rend() { return reverse_iterator(begin()); }
            const_reverse_iterator rend() const { return crend(); }
            const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }
            const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }
        };
    }
}

#endif // NTA_STATICVECTOR_H_INCLUDED