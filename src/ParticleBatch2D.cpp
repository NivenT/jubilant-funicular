#include <algorithm>

#include "ParticleBatch2D.h"

namespace nta {
    ParticleBatch2D::ParticleBatch2D() {
    }
    ParticleBatch2D::~ParticleBatch2D() {
        if (m_particles) {
            delete[] m_particles;
        }
    }
    void ParticleBatch2D::init(float il, float dr, float r, int mp, int tex, std::function<void(Particle2D&, float)> updateFunc) {
        m_initialLife = il;
        m_decayRate = dr;
        m_radius = r;
        m_maxParticles = mp;
        m_textureID = tex;
        m_updateFunction = updateFunc;
        m_particles = new Particle2D[mp];
        for (int i = 0; i < mp; i++) {
            m_particles[i].life = 0;
        }
    }
    int ParticleBatch2D::getFreeParticle() const {
        int minLife = 0;
        for (int i = 1; i < m_maxParticles; i++) {
            if (m_particles[i].life <= 0) {
                return i;
            } else if (m_particles[i].life < m_particles[minLife].life) {
                minLife = i;
            }
        }
        return minLife;
    }
    void ParticleBatch2D::addParticle(Particle2D p) {
        int index = getFreeParticle();
        m_particles[index] = p;
        m_particles[index].life = m_initialLife;
    }
    void ParticleBatch2D::draw(SpriteBatch& batch) const {
        for (int i = 0; i < m_maxParticles; i++) {
            if (m_particles[i].life > 0) {
                batch.addGlyph(glm::vec4(m_particles[i].center+glm::vec2(-m_radius,m_radius), 2.f*glm::vec2(m_radius)),
                               glm::vec4(0,0,1,1), m_textureID, 0, m_particles[i].color);
            }
        }
    }
    void ParticleBatch2D::update(float dt) {
        for (int i = 0; i < m_maxParticles; i++) {
            if (m_particles[i].life > 0) {
                m_updateFunction(m_particles[i], dt);
                m_particles[i].life -= m_decayRate*dt;
            }
        }
    }
    void ParticleBatch2D::clear() {
        for (int i = 0; i < m_maxParticles; i++) {
            m_particles[i].life = 0;
        }
    }
}
