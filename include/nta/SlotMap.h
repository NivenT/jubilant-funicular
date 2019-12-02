#ifndef NTA_SLOTMAP_H_INCLUDED
#define NTA_SLOTMAP_H_INCLUDED

#include "nta/Option.h"

namespace nta {
    namespace utils {
        /// An unsorted container supporting fast insertion/deletion without
        /// invalidating references to its elements.
        ///
        /// Warning: Doesn't really do error checking, so use carefully.
        /// \todo Add tests
        template<typename T, typename IndexType = std::size_t, typename GenType = uint16_t>
        class SlotMap {
        public:
            using index_type = IndexType;
            using size_type = IndexType;
            using gen_type = GenType;

            struct Key {
                index_type idx;
                gen_type gen;
            };
        private:
            void grow();

            Key* m_slots{};
            T* m_data{};

            size_type m_size{};
            size_type m_cap{};

            index_type m_free_head{};

            constexpr static size_type DEFAULT_CAP = 4;
        public:
            SlotMap() {}
            ~SlotMap();

            size_type size() const { return m_size; }
            size_type capacity() const { return m_cap; }
            bool is_empty() const { return m_size == 0; }
            bool empty() const { return is_empty(); }

            Option<T&> at(Key k);
            Option<T&> operator[](Key k) { return at(k); }

            Key add(const T& elem);
            Key push(const T& elem) { return add(elem); }
            template<typename... Args>
            Key emplace(Args&&... args) { return add(T(std::forward<Args>(args)...)); }

            void remove(Key k);
            void erase(Key k) { remove(k); }

            T* begin() { return &m_data[0]; }
            T* end() { return &m_data[m_size]; }
            const T* cbegin() const { return &m_data[0]; }
            const T* cend() const { return &m_data[m_size]; }
        };
        // These type names are hideous
        template<typename T, typename IndexType, typename GenType>
        SlotMap<T, IndexType, GenType>::~SlotMap() {
            std::destroy(m_data, m_data + m_size);
            std::free(m_data);
            std::destroy(m_slots, m_slots + m_cap);
            std::free(m_slots);
            m_size = m_cap = m_free_head = 0;
        }
        template<typename T, typename IndexType, typename GenType>
        Option<T&> SlotMap<T, IndexType, GenType>::at(SlotMap<T, IndexType, GenType>::Key k) {
            if (k.idx >= m_cap) return Option<T&>::none();
            const Key& key = m_slots[k.idx];
            if (key.gen != k.gen) return Option<T&>::none();
            if (key.idx >= m_size) return Option<T&>::none();
            return Option<T&>::some(m_data[key.idx]);
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::grow() {
            const size_type new_cap = m_cap == 0 ? DEFAULT_CAP : m_cap << 1;

            T* new_data = static_cast<T*>(std::aligned_alloc(alignof(T), 
                                                             sizeof(T) * new_cap));
            Key* new_slots = static_cast<Key*>(std::aligned_alloc(alignof(Key), 
                                                                  sizeof(Key) * new_cap));

            std::uninitialized_copy(m_data, m_data + m_cap, new_data);
            std::uninitialized_copy(m_slots, m_slots + m_cap, new_slots);

            std::destroy(m_data, m_data + m_cap);
            std::free(m_data);
            std::destroy(m_slots, m_slots + m_cap);
            std::free(m_slots);

            m_data = new_data;
            m_slots = new_slots;

            m_free_head = m_cap;
            for (index_type idx = m_cap; idx < new_cap; idx++) {
                m_slots[idx].gen = 0;
                // end of free list points to itself
                m_slots[idx].idx = std::min(idx+1, new_cap-1);
            }
            m_cap = new_cap;
        }
        template<typename T, typename IndexType, typename GenType>
        typename SlotMap<T, IndexType, GenType>::Key SlotMap<T, IndexType, GenType>::add(const T& elem) {
            if (m_size == m_cap) grow();
            new(&m_data[m_size]) T(elem);

            Key& k = m_slots[m_free_head];
            m_free_head = k.idx;
            k.idx = m_size++;

            return k;
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::remove(SlotMap<T, IndexType, GenType>::Key k) {
            if (k.idx >= m_cap) return;
            Key& key = m_slots[k.idx];
            if (key.gen != k.gen) return;
            if (key.idx >= m_size) return;

            m_size--;
            m_data[key.idx].~T();
            key.idx = m_free_head;
            key.gen++;
            m_free_head = k.idx;
        }
    }
}

#endif // NTA_SLOTMAP_H_INCLUDED