#ifndef NTA_SLOTMAP_H_INCLUDED
#define NTA_SLOTMAP_H_INCLUDED

#include "nta/Option.h"
#include "nta/IDFactory.h"
#include "nta/format.h"
#include "nta/utils.h"

namespace nta {
    namespace utils {
        /// A generational index
        template<typename IndexType = std::size_t, typename GenType = uint16_t>
        struct SlotMapKey {
            using index_type = IndexType;
            using gen_type = GenType;

            bool operator==(const SlotMapKey<IndexType, GenType> rhs) const {
                return idx == rhs.idx && gen == rhs.gen;
            }
            bool operator!=(const SlotMapKey<IndexType, GenType> rhs) const {
                return !(*this == rhs);
            }

            index_type idx;
            gen_type gen;
        };
        template<typename IndexType, typename GenType>
        struct Formatter<SlotMapKey<IndexType, GenType>> {
            std::string operator()(const SlotMapKey<IndexType, GenType>& arg) {
                return format("{ .idx = {}, .gen = {} }", arg.idx, arg.gen);
            }
        };
        template<typename IndexType, typename GenType>
        using GenIndex = SlotMapKey<IndexType, GenType>;
        /// Specialization for generation indices
        ///
        /// \todo Add tests
        template<typename IndexType, typename GenType>
        class IDFactory<GenIndex<IndexType, GenType>> {
        private:
            using value_type = GenIndex<IndexType, GenType>;
            /// IDs that were previously active but have since been freed
            std::vector<value_type> m_free;
            /// The smallest id that has never been assigned
            IndexType m_last_id;
        public:
            IDFactory() : m_last_id(NTA_INVALID_ID) {}
            /// Resets this to a new IDFactory
            void clear() { m_free.clear(); m_last_id = NTA_INVALID_ID; }
            /// Resets this to a new IDFactory
            void reset() { clear(); }

            /// Generates a new, unused ID
            value_type gen_id();
            /// Marks an id as free so that it can be reused
            ///
            /// id must not already be free
            void free_id(value_type id);
            /// calls free_id
            void free(value_type id) { free_id(id); }
            /// Returns whether or not an id is free
            bool is_free(value_type id) const;
            /// Returns whether or not the id is in use
            bool is_in_use(value_type id) const { return id.idx != NTA_INVALID_ID && !is_free(id); }

            /// Returns the number of active ids
            std::size_t get_count() const { return (std::size_t)m_last_id - m_free.size() - NTA_INVALID_ID; }
            /// Returns m_last_id
            IndexType get_last_id() const { return m_last_id; }

            /// Calls (and returns) gen_id
            value_type operator()() { return gen_id(); }
        };
        template<>
        template<typename IndexType, typename GenType>
        GenIndex<IndexType, GenType> IDFactory<GenIndex<IndexType, GenType>>::gen_id() {
            if (m_free.empty()) {
                return { .idx = ++m_last_id, .gen = 0 };
            } else {
                value_type ret = m_free.back();
                m_free.pop_back();
                ret.gen++;
                return ret;
            }
        }
        template<>
        template<typename IndexType, typename GenType>
        void IDFactory<GenIndex<IndexType, GenType>>::free_id(value_type id) {
            m_free.push_back(id);
        }
        template<>
        template<typename IndexType, typename GenType>
        bool IDFactory<GenIndex<IndexType, GenType>>::is_free(value_type id) const {
            if (id.idx > m_last_id) return true;
            for (const auto& id2 : m_free) {
                if (id == id2) return true;
            }
            return false;
        }
        /// An unsorted container supporting fast insertion/deletion without
        /// invalidating references to its elements.
        ///
        /// Warning: Doesn't really do error checking, so use carefully.
        template<typename T, typename IndexType = std::size_t, typename GenType = uint16_t>
        class SlotMap {
        public:
            using value_type = T;
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            using reference = T&;
            using const_reference = const T&;
            using index_type = IndexType;
            using size_type = IndexType;
            using gen_type = GenType;
            using Key = SlotMapKey<IndexType, GenType>;
            using storage_type = typename std::aligned_storage_t<sizeof(T), alignof(T)>;
        private:
            void grow() { reserve(m_cap == 0 ? DEFAULT_CAP : m_cap << 1); }
            void remove_impl(Key k, bool gen_bump);
            Option<reference> at_impl(Key k) const;
            reference _get_raw(index_type idx) const { 
                return reinterpret_cast<T*>(std::launder(&m_data[0]))[idx];
            }

            Key* m_slots{};
            storage_type* m_data{};
            index_type* m_slot_idxes{};

            size_type m_size{};
            size_type m_cap{};

            index_type m_free_head{};

            constexpr static size_type DEFAULT_CAP = 4;
        public:
            SlotMap() {}
            ~SlotMap();

            size_type size() const { return m_size; }
            size_type capacity() const { return m_cap; }
            size_type cap() const { return m_cap; }
            bool is_empty() const { return m_size == 0; }
            bool empty() const { return is_empty(); }

            Option<reference> at(Key k) { return at_impl(k); }
            Option<const_reference> at(Key k) const  { return at_impl(k); }
            Option<reference> operator[](Key k) { return at(k); }
            Option<const_reference> operator[](Key k) const { return at(k); }
            T& at_unsafe(Key k) { return at(k).unwrap(); }
            const T& at_unsafe(Key k) const { return at(k).unwrap(); }

            gen_type get_curr_gen(index_type idx) const { return idx < m_cap ? m_slots[idx].gen : 0; }
            gen_type get_curr_gen(Key k) const { return get_curr_gen(k.idx); }
            Key get_curr_key(index_type idx) const { return {.idx = idx, .gen = get_curr_gen(idx)}; }
            Key get_curr_key(Key k) const { return get_curr_key(k.idx); }
            bool is_free(index_type key_idx) const;
            bool is_free(Key k) const { return is_free(k.idx); }

            Key add(const T& elem);
            Key push(const T& elem) { return add(elem); }
            // This isn't really emplacing
            template<typename... Args>
            Key emplace(Args&&... args) { return add(T(std::forward<Args>(args)...)); }
            /// Checks that k refers to a free slot with the right generation
            ///
            /// Returns true on success
            bool insert(Key k, const T& elem);
            template<typename... Args>
            bool insert_emplace(Key k, Args&&... args);

            void reserve(size_type new_cap);

            void remove(Key k);
            /// Same thing as remove, but odes not bump the generation
            void deactivate(Key k);
            void erase(Key k) { remove(k); }
            void clear();

            iterator begin() { return &_get_raw(0); }
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
        // These type names are hideous
        template<typename T, typename IndexType, typename GenType>
        SlotMap<T, IndexType, GenType>::~SlotMap() {
            std::destroy(m_data, m_data + m_size);
            std::free(m_data);
            std::destroy(m_slots, m_slots + m_cap);
            std::free(m_slots);
            std::destroy(m_slot_idxes, m_slot_idxes + m_cap);
            std::free(m_slot_idxes);
            m_size = m_cap = m_free_head = 0;
        }
        template<typename T, typename IndexType, typename GenType>
        bool SlotMap<T, IndexType, GenType>::is_free(index_type key_idx) const {
            if (m_size == m_cap) return false;
            for (index_type idx = m_free_head;; idx = m_slots[idx].idx) {
                if (idx == key_idx) return true;
                if (idx == m_slots[idx].idx) break;
            }
            return false;
        }
        template<typename T, typename IndexType, typename GenType>
        Option<T&> SlotMap<T, IndexType, GenType>::at_impl(SlotMap<T, IndexType, GenType>::Key k) const {
            if (k.idx >= m_cap) return Option<T&>::none();
            const Key& key = m_slots[k.idx];
            if (key.gen != k.gen) return Option<T&>::none();
            if (is_free(k.idx)) return Option<T&>::none();
            return Option<T&>::some(_get_raw(key.idx));
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::reserve(size_type new_cap) {
            if (new_cap <= m_cap) return;

            storage_type* new_data;
            Key*          new_slots;
            index_type*   new_idxes;

            new_data = static_cast<storage_type*>(std::aligned_alloc(alignof(T), 
                                                                     sizeof(T) * new_cap));
            new_slots = static_cast<Key*>(std::aligned_alloc(alignof(Key), 
                                                             sizeof(Key) * new_cap));
            new_idxes = static_cast<index_type*>(std::aligned_alloc(alignof(index_type),
                                                                    sizeof(index_type) * new_cap));
    
            std::uninitialized_copy(m_data, m_data + m_size, new_data);
            std::uninitialized_copy(m_slots, m_slots + m_cap, new_slots);
            std::uninitialized_copy(m_slot_idxes, m_slot_idxes + m_size, new_idxes);

            std::destroy(m_data, m_data + m_size);
            std::free(m_data);
            std::destroy(m_slots, m_slots + m_cap);
            std::free(m_slots);
            std::destroy(m_slot_idxes, m_slot_idxes + m_size);
            std::free(m_slot_idxes);

            m_data = new_data;
            m_slots = new_slots;
            m_slot_idxes = new_idxes;

            for (index_type idx = m_cap; idx < new_cap; idx++) {
                m_slots[idx].gen = 0;
                // end of free list points to itself
                m_slots[idx].idx = std::min(idx+1, new_cap-1);
            }
            if (m_size == m_cap) {
                m_free_head = m_cap;
            } else for (index_type idx = m_free_head;; idx = m_slots[idx].idx) {
                if (idx == m_slots[idx].idx) {
                    m_slots[idx].idx = m_cap;
                    break;
                }
            }
            m_cap = new_cap;
        }
        template<typename T, typename IndexType, typename GenType>
        template<typename... Args>
        bool SlotMap<T, IndexType, GenType>::insert_emplace(Key k, Args&&... args) {
            if (m_size == m_cap) return false;

            index_type prev_idx;
            for (index_type idx = m_free_head;; idx = m_slots[idx].idx) {
                if (idx == k.idx) break;
                if (idx == m_slots[idx].idx) return false;
                prev_idx = idx;
            }
            if (k.gen != m_slots[k.idx].gen) return false;

            if (m_size == m_cap) grow();
            new(&m_data[m_size]) T(std::forward<Args>(args)...);

            if (k.idx == m_free_head) {
                m_free_head = m_slots[k.idx].idx;
            } else if (k.idx == m_slots[k.idx].idx) {
                m_slots[prev_idx].idx = prev_idx;
            } else {
                m_slots[prev_idx].idx = m_slots[k.idx].idx;
            }
            m_slots[k.idx].idx = m_size;
            m_slot_idxes[m_size++] = k.idx;
            return true;
        }
        template<typename T, typename IndexType, typename GenType>
        bool SlotMap<T, IndexType, GenType>::insert(Key k, const T& elem) {
            return insert_emplace(k, elem);
        }
        template<typename T, typename IndexType, typename GenType>
        typename SlotMap<T, IndexType, GenType>::Key SlotMap<T, IndexType, GenType>::add(const T& elem) {
            if (m_size == m_cap) grow();
            new(&m_data[m_size]) T(elem);

            index_type key_idx = m_free_head;
            m_free_head = m_slots[key_idx].idx;

            m_slots[key_idx].idx = m_size;
            m_slot_idxes[m_size++] = key_idx;
            return {.idx = key_idx, .gen = m_slots[key_idx].gen};
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::remove_impl(SlotMap<T, IndexType, GenType>::Key k, bool gen_bump) {
            if (k.idx >= m_cap) return;
            Key& key = m_slots[k.idx];
            if (key.gen != k.gen) return;
            if (is_free(k.idx)) return;

            m_size--;
            _get_raw(key.idx).~T();

            std::swap(m_data[key.idx], m_data[m_size]);

            m_slots[m_slot_idxes[m_size]].idx = key.idx;
            m_slot_idxes[key.idx] = m_slot_idxes[m_size];

            key.idx = m_free_head;
            if (gen_bump) key.gen++;
            m_free_head = k.idx;
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::remove(SlotMap<T, IndexType, GenType>::Key k) {
            remove_impl(k, true);
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::deactivate(SlotMap<T, IndexType, GenType>::Key k) {
            remove_impl(k, false);
        }
        template<typename T, typename IndexType, typename GenType>
        void SlotMap<T, IndexType, GenType>::clear() {
            for (index_type i = 0; i < m_cap; i++) remove({.idx = i, .gen = m_slots[i].gen});
        }  
    }
}

namespace std {
    template<typename IndexType, typename GenType>
    struct hash<nta::utils::SlotMapKey<IndexType, GenType>> {
        std::size_t operator()(const nta::utils::SlotMapKey<IndexType, GenType> key) const {
            return nta::utils::hash_combine(std::hash<IndexType>()(key.idx),
                                            std::hash<GenType>()(key.gen));
        }
    };
};

#endif // NTA_SLOTMAP_H_INCLUDED