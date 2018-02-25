#include "nta/ThreadPool.h"

namespace nta {
    ThreadPool::ThreadPool(size_t num_threads) : m_workers(num_threads), m_kill(false),
        m_available(num_threads) {
        m_dispatch_thread = std::thread([this]{ dispatcher(); });
        for (size_t id = 0; id < num_threads; id++) {
            m_workers[id].t = std::thread([this, id]{ worker(id); });
        }
    }
    bool ThreadPool::allWorkFinished() {
        for (size_t  i = 0; i < m_workers.size(); i++) {
            if (!m_workers[i].free) return false;
        }
        return m_funcs.empty();
    }
    size_t ThreadPool::getAvailableWorker() {
        m_available.wait();
        for (size_t i = 0; i < m_workers.size(); i++) {
            if (m_workers[i].free) {
                m_workers[i].free = false;
                return i;
            }
        }
        // Should never get here
    }
    void ThreadPool::dispatcher() {
        /// \todo Can I reorganize to replace with a while(!kill)?
        while (true) {
            m_scheduled.wait();
            if (m_kill) break;
            size_t id = getAvailableWorker();

            m_empty.m.lock();
            m_workers[id].thunk = m_funcs.front();
            m_funcs.pop();
            m_empty.m.unlock();

            m_workers[id].task.signal();
        }
        // Notify workers to die
        for (auto& w : m_workers) {
            w.task.signal();
        }
    }
    void ThreadPool::worker(size_t wid) {
        auto& me = m_workers[wid];
        while (true) {
            me.task.wait();
            if (m_kill) break;
            me.thunk();
            me.free = true;

            m_available.signal();

            m_empty.m.lock();
            m_empty.cv.notify_all();
            m_empty.m.unlock();
        }
    }
    void ThreadPool::schedule(const Thunk& thunk) {
        m_empty.m.lock();
        m_funcs.push(thunk);
        m_empty.m.unlock();
        m_scheduled.signal();
    }
    void ThreadPool::wait() {
        std::lock_guard<std::mutex> lg(m_empty.m);
        m_empty.cv.wait(m_empty.m, [this]{return allWorkFinished();});
    }
    ThreadPool::~ThreadPool() {
        wait();

        m_kill = true;
        // make sure everyone gets the kill notification
        m_scheduled.signal();

        m_dispatch_thread.join();
        for (auto& w : m_workers) {
            w.t.join();
        }
    }
}