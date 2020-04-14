#include "nta/Observer.h"

namespace nta {
	/*
	void Subject::notify(const Message& event) {
		for (ObserverNode* curr = m_head; curr; curr = m_head->next) {
			curr->data->onNotify(event);
		}
	}
	void Subject::subscribe(Observer* obs) {
		m_head = new ObserverNode(obs, m_head);
	}
	void Subject::unsubscribe(Observer* obs) {
		ObserverNode** curr = &m_head;
		while ((*curr) && (*curr)->data != obs) {
			curr = &(*curr)->next;
		}
		if (*curr) *curr = (*curr)->next;
	}
	*/
}