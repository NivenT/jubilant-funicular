#ifndef NTA_OBSERVER_H_INCLUDED
#define NTA_OBSERVER_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
    /// Something someone needs to be notified about
    struct Event {
        Event() : type(0), data(nullptr) {}
        Event(uint64_t type) : type(type), data(nullptr) {}
        Event(uint64_t type, void* data) : type(type), data(data) {}

        uint64_t operator&(uint64_t rhs) const { return type & rhs; }

        /// bit array (or bit mask) specifying type of event (semantic meaning is user-defined)
        uint64_t type;
        /// arbitrary data associated with this Event
        void* data;
    };
    /// Receives notifications
    class Observer {
    public:
        virtual ~Observer() {}
        /// (Synchronously) Handles event the Observer just became aware of
        /// \todo Take Entity as well whenever I write an Entity-Component System
        virtual void onNotify(const Event&) = 0;
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
        virtual void notify(const Event&);
    public:
        virtual ~Subject() { if (m_head) delete m_head; }
        /// Adds Observer to notification list
        virtual void subscribe(Observer*);
        /// Removes Observer from notification list
        virtual void unsubscribe(Observer*);
    };
}

#endif // NTA_OBSERVER_H_INCLUDED
