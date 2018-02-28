#include "nta/ThreadPool.h"

namespace nta {
    namespace utils {
        Semaphore::Semaphore(int value) : m_value(value) {
        }
        void Semaphore::wait() {
            std::lock_guard<std::mutex> lg(m_mutex);
            m_cv.wait(m_mutex, [this]{return m_value > 0;});
            m_value--;
        }
        void Semaphore::signal() {
            std::lock_guard<std::mutex> lg(m_mutex);
            if (m_value++ == 0) m_cv.notify_all();
        }
    }
}