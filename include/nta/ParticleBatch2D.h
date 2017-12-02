#ifndef PARTICLEBATCH2D_H_INCLUDED
#define PARTICLEBATCH2D_H_INCLUDED

#include "SpriteBatch.h"

namespace nta {
    ///Represents a simple 2d particle
    struct Particle2D {
                                    Particle2D(){
                                    }
                                    Particle2D(crvec2 c, crvec2 v, crvec4 col) : center(c), velocity(v), color(col) {
                                    }
        glm::vec2                   center;
        glm::vec2                   velocity;
        glm::vec4                   color;
        float                       life;
    };
    ///Represents a batch of particles of the same "type"
    class ParticleBatch2D {
    private:
        ///returns index of particle with the minimum life
        int                                         getFreeParticle() const;
        ///the function used to update the particles
        std::function<void(Particle2D&, float)>     m_updateFunction;
        ///the particles themselves
        Particle2D*                                 m_particles = nullptr;
        ///the life every particle starts out with
        float                                       m_initialLife;
        ///the rate with which particles lose life
        float                                       m_decayRate;
        ///the radius of an individual particle
        float                                       m_radius;
        ///maximum number of particles allowed
        int                                         m_maxParticles;
        ///texture used by the particles
        int                                         m_textureID;
    public:
        ///basic constructor
                                                    ParticleBatch2D();
        ///deletes particles
                                                    ~ParticleBatch2D();
        ///initializes particle batch by specifying properties
        void                                        init(float il, float dr, float r, int mp, int tex, std::function<void(Particle2D&, float)> updateFunc);
        ///adds a particle to the batch
        void                                        addParticle(Particle2D p);
        ///draws all the particles
        void                                        draw(SpriteBatch& batch) const;
        ///updates the particles
        void                                        update(float dt);
        ///removes all particles
        void                                        clear();
    };
};

#endif // PARTICLEBATCH2D_H_INCLUDED
