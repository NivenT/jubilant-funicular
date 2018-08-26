#ifndef NTA_IDFACTORY_H_INCLUDED
#define NTA_IDFACTORY_H_INCLUDED

#include <stack>

#include "nta/MyEngine.h"

namespace nta {
	namespace utils {
		/// Class for generating unique (integral) IDs
		template<typename T>
		class IDFactory {
		private:
			/// IDs that were previously active but have since been freed
			std::stack<T> m_free;
			/// The smallest id that has never been assigned
			T m_next_id;
		public:
			IDFactory() : m_next_id(0) {}
			/// Resets this to a new IDFactory
			void clear() { m_free = std::stack<T>(); m_next_id = 0; }
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

			/// Returns the number of active ids
			std::size_t get_count() const { return m_next_id - m_free.size(); }

			/// Calls (and returns) gen_id
			T operator()() { return gen_id(); }
		};

		template<typename T>
		T IDFactory<T>::gen_id() {
			if (m_free.empty()) {
				return m_next_id++;
			} else {
				T ret = m_free.top();
				m_free.pop();
				return ret;
			}
		}

		template<typename T>
		void IDFactory<T>::free_id(T id) {
			m_free.push(id);
		}
	}
}

#endif // NTA_IDFACTORY_H_INCLUDED