#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include "nta/MyEngine.h"

#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

typedef std::function<void(void)> Thunk;

/// \todo Make sure this code is OS-agnostic
namespace nta {
    class Semaphore {
    private:
        /// Thread safety stuff
        std::mutex m;
        std::condition_variable_any cv;
        /// The number of resources available
        int value;
    public:
        /// Constructs Semaphore with given value
        Semaphore(int value = 0);
        /// Halts until value of Semaphore is > 0
        void wait();
        /// Increments value of Semaphore
        void signal();

        Semaphore(const Semaphore&) = delete;
        const Semaphore& operator=(const Semaphore&) = delete;
    };
    /// Collection of threads for running scheduled functions
    class ThreadPool {
    private:
        /// One thread available to the pool
        struct worker {
            /// The thread owned by the worker
            std::thread t;
            /// The function the worker is currently scheduled to execute
            Thunk thunk;
            /// Whether or not the worker is free to take on a new function
            std::atomic<bool> free;
            Semaphore task;

            worker() : free(true) {}
        };

        /// Used to keep track of when the pool is empty
        struct {
            std::condition_variable_any cv;
            std::mutex m;
        } m_empty;

        /// Sign to kill the pool
        std::atomic<bool> m_kill;
        /// Record of scheduled workers
        Semaphore m_scheduled;
        /// Record of available workers
        Semaphore m_available;
        /// The functions to be executed
        std::queue<Thunk> m_funcs;
        /// The thread for managing the pool
        std::thread m_dispatch_thread;

        void dispatcher();
        void worker(size_t wid);
        size_t getAvailableWorker();
        bool allWorkFinished();
    public:
        /// Constructs a ThreadPool with specified number of threads
        ThreadPool(size_t num_threads);
        ~ThreadPool();
        /// Schedules a function to be executed
        void schedule(const Thunk& thunk);
        /// Waits for all scheduled functions to finish execution
        void wait();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
    };
}

#endif // THREADPOOL_H_INCLUDED