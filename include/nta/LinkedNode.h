#ifndef NTA_LINKEDNODE_H_INCLUDED
#define NTA_LINKEDNODE_H_INCLUDED

#include <functional>

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
				using value_type = T;
				using difference_type = std::ptrdiff_t;
				using pointer = T*;
				using reference = T&;
				using iterator_category = std::forward_iterator_tag;
			private:
				LinkedNode* m_node;
			public:
				iterator() {}
				iterator(LinkedNode* node) : m_node(node) {}
				reference operator*() const {
					return m_node->data;
				}
				pointer operator->() const {
					return &m_node->data;
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

			LinkedNode(T d) : data(d) {}
	        LinkedNode(T d, LinkedNode* nxt) : next(nxt), data(d) {}
	        ~LinkedNode() { if (next) delete next; next = nullptr; }

	        iterator begin() {
	        	return iterator(this);
	        }
	        iterator end() {
	        	return iterator(nullptr);
	        }

	        std::size_t size() const {
	        	std::size_t len = 0;
	        	const LinkedNode* curr = this;
	        	while (curr && ++len) curr = curr->next;
	        	return len;
	        }

	        static void remove(LinkedNode*& head, T d) {
	        	LinkedNode** curr = &head;
				while ((*curr) && (*curr)->data != d) {
					curr = &(*curr)->next;
				}
				if (*curr) *curr = (*curr)->next;
	        }
	        static void remove(LinkedNode*& node) {
	        	if (node) node = node->next;
	        }
	        /// returns true on success
	        static bool remove(LinkedNode*& head, std::function<bool(const T&)> pred) {
	        	LinkedNode** curr = &head;
	        	while (*curr && !pred((*curr)->data)) {
	        		curr = &(*curr)->next;
	        	}
	        	if (*curr) {
	        		*curr = (*curr)->next;
	        		return true;
	        	}
	        	return false;
	        }

	        /// The next node in the linked list
	        LinkedNode* next = nullptr;
	        /// The (pointer to the) data associated to this node
	        T data;
		};
	}
}

#endif // NTA_LINKEDNODE_H_INCLUDED