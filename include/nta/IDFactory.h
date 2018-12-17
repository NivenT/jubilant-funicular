#ifndef NTA_IDFACTORY_H_INCLUDED
#define NTA_IDFACTORY_H_INCLUDED

#define NTA_INVALID_ID 0

#include <vector>

#include "nta/MyEngine.h"

namespace nta {
	namespace utils {
		/// Class for generating unique (integral) IDs
		template<typename T>
		class IDFactory {
		private:
			/// IDs that were previously active but have since been freed
			std::vector<T> m_free;
			/// The smallest id that has never been assigned
			T m_last_id;
		public:
			IDFactory() : m_last_id(NTA_INVALID_ID) {}
			/// Resets this to a new IDFactory
			void clear() { m_free.clear(); m_last_id = NTA_INVALID_ID; }
			/// Resets this to a new IDFactory
			void reset() { clear(); }

			/// Generates a new, unused ID
			T gen_id();
			/// Marks an id as free so that it can be reused
			///
			/// id must not already be free
			void free_id(T id);
			/// calls free_id
			void free(T id) { free_id(id); }
			/// Returns whether or not an id is free
			bool is_free(T id) const;
			/// Returns whether or not the id is in use
			bool is_in_use(T id) const { return id != NTA_INVALID_ID && !is_free(id); }

			/// Returns the number of active ids
			std::size_t get_count() const { return (std::size_t)m_last_id - m_free.size() - NTA_INVALID_ID; }
			/// Returns m_last_id
			T get_last_id() const { return m_last_id; }

			/// Calls (and returns) gen_id
			T operator()() { return gen_id(); }
		};
		template<typename T>
		T IDFactory<T>::gen_id() {
			if (m_free.empty()) {
				return ++m_last_id;
			} else {
				T ret = m_free.back();
				m_free.pop_back();
				return ret;
			}
		}
		template<typename T>
		void IDFactory<T>::free_id(T id) {
			m_free.push_back(id);
		}
		template<typename T>
		bool IDFactory<T>::is_free(T id) const {
			if (id > m_last_id) return true;
			for (const auto& id2 : m_free) {
				if (id == id2) return true;
			}
			return false;
		}
	}
}

#endif // NTA_IDFACTORY_H_INCLUDED