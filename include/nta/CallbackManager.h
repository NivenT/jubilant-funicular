#ifndef CALLBACKMANAGER_H_INCLUDED
#define CALLBACKMANAGER_H_INCLUDED

#include <map>

#include "nta/ThreadPool.h"

namespace nta {
    /// Allows scheduling of functions to be called at a later point in time
    /// \todo Add ability to base timing off of actual time instead of frames
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

        static void dispatch();

        static std::map<uint64_t, event> m_active;
        static std::map<uint64_t, std::vector<event*>> m_queue;

        static utils::ThreadPool m_pool;
        static std::thread m_dispatcher;
        static std::atomic<uint64_t> m_next;
        static std::mutex m_mutex;
        static std::condition_variable_any m_cv;
        static bool m_working;
        static uint64_t m_curr_frame;
    public:
        static uint64_t setInterval(const Thunk& thunk, uint64_t when, uint64_t period);
        static uint64_t setTimeout(const Thunk& thunk, uint64_t when);
        static void clear(uint64_t id);
        static void increment_frame();
        static void destroy();
    };
}

#endif // CALLBACKMANAGER_H_INCLUDED