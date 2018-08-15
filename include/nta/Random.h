#ifndef NTA_RANDOM_H_INCLUDED
#define NTA_RANDOM_H_INCLUDED

#include <random>

#ifndef RAND_MAX
#define RAND_MAX 0x7FFF
#endif // RAND_MAX

namespace nta {
    /// Used for generating random numbers
    class Random {
    private:
        static std::default_random_engine   generator;
    public:
        /// initializes random number generation
        static void init();
        /// randomly returns true or false
        static bool randBool() {return rand()%2==0;}
        /// returns a random int in the specified range exclusive (uniform distribution)
        static long randInt(long min, long max);
        static long randInt(long max) {return randInt(0,max);}
        static long randInt() {return randInt(0,RAND_MAX);}
        /// returns a random float in the specified range (uniform distribution)
        static float randFloat(float min, float max);
        static float randFloat(float max) {return randFloat(0,max);}
        static float randFloat() {return randFloat(0,1);}
        /// returns a random float using the specified distribution
        static float randGaussian(float mean, float sd);
        /// returns the random number generator
        static std::default_random_engine getRNG() {return generator;}
    };
}

#endif // NTA_RANDOM_H_INCLUDED
