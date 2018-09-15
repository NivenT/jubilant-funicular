#ifndef NTA_OBSERVER_H_INCLUDED
#define NTA_OBSERVER_H_INCLUDED

#include "nta/Message.h"
#include "nta/LinkedNode.h"

namespace nta {
    /// Receives notifications
    class Observer {
    public:
        virtual ~Observer() {}
        /// (Synchronously) Handles event the Observer just became aware of
        virtual void onNotify(const Message&) = 0;
    };
    /// Sends notifications
    class Subject {
    private:
        typedef utils::LinkedNode<Observer*> ObserverNode;
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
