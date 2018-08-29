#ifndef NTA_LINKEDNODE_H_INCLUDED
#define NTA_LINKEDNODE_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
	namespace utils {
		/// A generic linked list
		// Why not use std::forward_list? Because I forgot it existed...
		template<typename T>
		struct LinkedNode {
		    /// Custom iterator for looping over the data in the linked list
			class iterator {
			public:
				using value_type = T*;
				using difference_type = std::ptrdiff_t;
				using pointer = LinkedNode*;
				using reference = T*&;
				using iterator_category = std::forward_iterator_tag;
			private:
				pointer m_node;
			public:
				iterator() {}
				iterator(pointer node) : m_node(node) {}
				reference operator*() const {
					return m_node->data;
				}
				reference operator->() const {
					return operator*();
				}
				iterator& operator++() {
					m_node = m_node->next;
					return *this;
				}
				iterator operator++(int) {
					auto ret = *this;
					operator++();
					return ret;
				}
				bool operator==(const iterator& rhs) const {
					return m_node == rhs.m_node;
				}
				bool operator!=(const iterator& rhs) const {
					return !operator==(rhs);
				}
			};

			LinkedNode(T* d) : data(d) {}
	        LinkedNode(T* d, LinkedNode* nxt) : next(nxt), data(d) {}
	        ~LinkedNode() { if (next) delete next; }

	        iterator begin() {
	        	return iterator(this);
	        }
	        iterator end() {
	        	return iterator(nullptr);
	        }

	        static void remove(LinkedNode** head, T* d) {
	        	LinkedNode** curr = head;
				while ((*curr) && (*curr)->data != d) {
					curr = &(*curr)->next;
				}
				if (*curr) *curr = (*curr)->next;
	        }
	        static void remove(LinkedNode*& node) {
	        	if (node) node = node->next;
	        }

	        /// The next node in the linked list
	        LinkedNode* next = nullptr;
	        /// The (pointer to the) data associated to this node
	        T* data = nullptr;
		};
	}
}

#endif // NTA_LINKEDNODE_H_INCLUDED