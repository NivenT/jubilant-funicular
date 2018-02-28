#include "nta/CallbackManager.h"

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
        if (empty || e.when < m_curr_frame) {
            m_cv.notify_one();
        }
        return e.id;
    }
    uint64_t CallbackManager::setTimeout(const Thunk& thunk, uint64_t when) {
        setInterval(thunk, when, 0);
    }
    void CallbackManager::init() {
        m_dispatcher = std::thread([&]{dispatch();});
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
                if (m_curr_frame < m_queue.begin()->first) {
                    m_mutex.unlock();
                    break;
                }

                auto& es = m_queue.begin()->second;
                while (!es.empty()) {
                    event* e = es[0];

                    m_pool.schedule(e->thunk);
                    if (e->period == 0) {
                        m_mutex.unlock();
                        clear(e->id);
                        m_mutex.lock();
                    } else {
                        es.erase(es.begin());
                        e->when += e->period;
                        m_queue[e->when].push_back(e);
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
        m_active.erase(id);
    }
    void CallbackManager::destroy() {
        m_mutex.lock();
        m_working = false;
        m_cv.notify_one();
        m_mutex.unlock();

        m_dispatcher.join();
        m_pool.wait();
    }
    void CallbackManager::increment_frame() {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_curr_frame++;
    }
}
