#include "nta/ParticleEngine2D.h"

namespace nta {
    ParticleEngine2D::ParticleEngine2D() {
    }
    ParticleEngine2D::~ParticleEngine2D() {
        for (auto& pBatch : m_batches) {
            delete pBatch;
        }
        m_batches.clear();
    }
    void ParticleEngine2D::addBatch(ParticleBatch2D* batch) {
        m_batches.push_back(batch);
    }
    void ParticleEngine2D::draw(SpriteBatch& batch) const {
        for (auto& pBatch : m_batches) {
            pBatch->draw(batch);
        }
    }
    void ParticleEngine2D::update(float dt) const {
        for (auto& pBatch : m_batches) {
            pBatch->update(dt);
        }
    }
}
