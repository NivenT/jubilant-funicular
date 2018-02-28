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
    void CallbackManager::increment_frame() {
        m_curr_frame++;
    }
}