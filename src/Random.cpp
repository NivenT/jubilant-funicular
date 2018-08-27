#include <ctime>
#include <cstdlib>

#include "nta/Random.h"

namespace nta {
    std::default_random_engine Random::m_generator;
    void Random::init() {
        std::srand(std::time(nullptr));
        m_generator = std::default_random_engine(std::time(nullptr));
    }
    long Random::randInt(long min, long max) {
        //+1 to prevent the case of rand() == RAND_MAX
        return rand()*double(max-min)/((double)RAND_MAX+1) + min;
    }
    float Random::randFloat(float min, float max) {
        return rand()*(max-min)/RAND_MAX + min;
    }
    float Random::randGaussian(float mean, float sd) {
        std::normal_distribution<float> distribution(mean,sd);
        return distribution(m_generator);
    }
}
