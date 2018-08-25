#ifndef NTA_OBSERVER_H_INCLUDED
#define NTA_OBSERVER_H_INCLUDED

#include "nta/Message.h"

namespace nta {
    /// Receives notifications
    class Observer {
    public:
        virtual ~Observer() {}
        /// (Synchronously) Handles event the Observer just became aware of
        /// \todo (?) Take Entity as well whenever I write an Entity-Component System
        virtual void onNotify(const Message&) = 0;
    };
    /// Sends notifications
    class Subject {
    private:
        /// Linked list of Observers
        struct ObserverNode {
            ObserverNode(Observer* obs) : observer(obs) {}
            ObserverNode(Observer* obs, ObserverNode* nxt) : next(nxt), observer(obs) {}
            ~ObserverNode() { if (next) delete next; }

            /// The next node in the linked list
            ObserverNode* next = nullptr;
            /// The Observer associated to this node
            Observer* observer = nullptr;
        };

        /// A linked list of Observers to notify
        ObserverNode* m_head = nullptr;
    protected:
        virtual void notify(const Message&);
    public:
        virtual ~Subject() { if (m_head) delete m_head; }
        /// Adds Observer to notification list
        virtual void subscribe(Observer*);
        /// Removes Observer from notification list
        virtual void unsubscribe(Observer*);
    };
}

#endif // NTA_OBSERVER_H_INCLUDED
