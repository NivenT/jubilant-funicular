#include "nta/CallbackManager.h"
#include "nta/Logger.h"

namespace nta {
    std::map<uint64_t, CallbackManager::event>                  CallbackManager::m_active;
    std::map<uint64_t, std::vector<CallbackManager::event*>>    CallbackManager::m_queue;
    utils::ThreadPool                                           CallbackManager::m_pool(8);
    std::thread                                                 CallbackManager::m_dispatcher;
    std::atomic<uint64_t>                                       CallbackManager::m_next(0);
    std::mutex                                                  CallbackManager::m_mutex;
    std::condition_variable_any                                 CallbackManager::m_cv;
    bool                                                        CallbackManager::m_working(true);
    uint64_t                                                    CallbackManager::m_curr_frame(0);
    
    uint64_t CallbackManager::setInterval(const Thunk& thunk, uint64_t when, uint64_t period) {
        std::lock_guard<std::mutex> lg(m_mutex);
        
        event& e = m_active[m_next];
        e.id = m_next++;
        e.when = m_curr_frame + when;
        e.period = period;
        e.thunk = thunk;
    
        bool empty = m_queue.empty();
        m_queue[e.when].push_back(&e);
        // Second half of || is impossible, but the analagous check
        //   won't be impossible if we basing time on an actual clock
        if (empty || e.when < m_curr_frame) {
            m_cv.notify_one();
        }
        return e.id;
    }
    uint64_t CallbackManager::setTimeout(const Thunk& thunk, uint64_t when) {
        return setInterval(thunk, when, 0);
    }
    void CallbackManager::init() {
        Logger::writeToLog("Initializing CallbackManager...");
        m_dispatcher = std::thread([&]{dispatch();});
        m_next = m_curr_frame = 0;
        m_working = true;
        Logger::writeToLog("Initialized CallbackManager");
    }
    void CallbackManager::dispatch() {
        while (true) {
            m_mutex.lock();
            m_cv.wait(m_mutex, [&](){return !m_working || !m_queue.empty();});
            if (!m_working) break;
            m_cv.wait(m_mutex, [&](){return m_curr_frame >= m_queue.begin()->first;});
            m_mutex.unlock();

            while (true) {
                m_mutex.lock();
                if (m_queue.empty() || m_curr_frame < m_queue.begin()->first) {
                    m_mutex.unlock();
                    break;
                }

                auto& es = m_queue.begin()->second;
                while (!es.empty()) {
                    event* e = es[0];
                    m_pool.schedule(e->thunk);

                    m_mutex.unlock();
                    clear(e->id);
                    m_mutex.lock();

                    if (e->period > 0) {
                        e->when += e->period;
                        m_queue[e->when].push_back(e);
                        m_active[e->id] = *e;
                    }
                }
                m_mutex.unlock();
            }
        }
    }
    void CallbackManager::clear(uint64_t id) {
        std::lock_guard<std::mutex> lg(m_mutex);
        if (m_active.find(id) == m_active.end()) return;

        event& e = m_active[id];
        auto& es = m_queue[e.when];
        for (size_t i = 0; i < es.size(); i++) {
            if (es[i] == &e) {
                es.erase(es.begin() + i);
                break;
            }
        }
        if (es.empty()) m_queue.erase(e.when);
        m_active.erase(id);
    }
    void CallbackManager::destroy() {
        Logger::writeToLog("Destroying CallbackManager...");
        m_mutex.lock();
        m_working = false;
        m_cv.notify_one();
        m_mutex.unlock();

        m_dispatcher.join();
        m_pool.wait();

        m_curr_frame = 0;
        m_queue.clear();
        m_active.clear();
        Logger::writeToLog("Destroyed CallbackManager");
    }
    void CallbackManager::increment_frame() {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_curr_frame++;
        if (!m_queue.empty() && m_queue.begin()->first < m_curr_frame) m_cv.notify_one();
    }
}
