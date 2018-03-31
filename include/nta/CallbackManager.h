#ifndef CALLBACKMANAGER_H_INCLUDED
#define CALLBACKMANAGER_H_INCLUDED

#include <map>

#include "nta/ThreadPool.h"

namespace nta {
    /// Allows scheduling of functions to be called at a later point in time
    /// \todo Add ability to base timing off of actual time instead of frames
    /// \todo make non-static (ideally, one CallbackManager per ScreenManager)
    class CallbackManager {
    private:
        /// A function to run at a certain time with some regularity
        struct event {
            /// id of event
            uint64_t id;
            /// How many frames to run it in
            uint64_t when;
            /// How many frames between consecutive runs
            uint64_t period;
            /// Function to execute
            Thunk thunk;
        };

        /// Function run by m_dispatcher
        /// Handles calling of the various events at the right times
        static void dispatch();
        /// Removes event from m_queue but not m_active (assumes id is valid)
        static void dequeue(uint64_t id);

        /// The events waiting to happen (key is event.id)
        static std::map<uint64_t, event> m_active;
        /// "Priority queue" of events ordered by when they should occur
        static std::map<uint64_t, std::vector<event*>> m_queue;

        /// ThreadPool used for scheduling events to happen
        static utils::ThreadPool m_pool;
        /// Thread used to dispatch events
        static std::thread m_dispatcher;
        /// id given to the next created event
        static std::atomic<uint64_t> m_next;
        // It's been a while since I've written parallel code, but am I overusing this?
        static std::mutex m_mutex;
        static std::condition_variable_any m_cv;
        /// Global flag saying whether or not the CallbackManager has been destroyed
        static bool m_working;
        /// The current frame
        static uint64_t m_curr_frame;
    public:
        /// Schedules a function to be called after when frames and then repeatedly after
        ///   each subsequent period frames
        static uint64_t setInterval(const Thunk& thunk, uint64_t when, uint64_t period);
        /// Schedules a function to be called once after when frames and then repeatedly after
        static uint64_t setTimeout(const Thunk& thunk, uint64_t when);
        /// Changes when an event is next scheduled to run
        static bool delay(uint64_t id, uint64_t when, bool absolute = true);
        /// Initializes the CallbackManager
        static void init();
        /// Removes the event with the given id
        static void clear(uint64_t id);
        /// Informs CallbackManager that a frame has passed
        static void increment_frame();
        /// Destroys the CallbackManager
        static void destroy();
    };
}

#endif // CALLBACKMANAGER_H_INCLUDED