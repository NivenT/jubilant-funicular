#ifndef PARTICLEENGINE2D_H_INCLUDED
#define PARTICLEENGINE2D_H_INCLUDED

#include <vector>

#include "ParticleBatch2D.h"

namespace nta {
    ///Responsible for handling multiple particle batches
    class ParticleEngine2D {
    private:
        ///the particle batches
        std::vector<ParticleBatch2D*>       m_batches;
    public:
        ///basic constructor
                                            ParticleEngine2D();
        ///deletes batches
                                            ~ParticleEngine2D();
        ///adds a batch
        void                                addBatch(ParticleBatch2D* batch);
        ///renders all batches
        void                                draw(SpriteBatch& batch) const;
        ///updates all batches
        void                                update(float dt) const;
    };
}

#endif // PARTICLEENGINE2D_H_INCLUDED
