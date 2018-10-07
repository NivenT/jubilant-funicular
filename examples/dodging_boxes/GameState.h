#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <nta/ECS.h>
#include <nta/SpriteBatch.h>
#include <nta/ResourceManager.h>
#include <nta/Random.h>

class BallComponent : public nta::Component {
private:
    glm::vec4 m_col;
    glm::vec2 m_cen;
    glm::vec2 m_vel;
    float m_rad;

    bool m_touched_ground = false;
public:
    BallComponent(nta::crvec2 cen, float rad, nta::crvec4 col) : m_col(col), m_rad(rad), m_cen(cen) {}
    bool has_touched_ground() const { return m_touched_ground; }
    void draw(nta::SpriteBatch& batch) const {
        static nta::GLTexture tex = nta::ResourceManager::getTexture("circle.png").get_data();
        batch.addGlyph(glm::vec4(m_cen.x - m_rad, m_cen.y + m_rad, m_rad*2.f, m_rad*2.f),
                       glm::vec4(0, 0, 1, 1), tex.id, m_col);
    }
    void update(float dt) {
        m_vel.y -= 9.8f * dt;
        m_cen += m_vel * dt;

        if (m_cen.y - m_rad < -100) {
            m_cen.y = m_rad - 100;
            m_touched_ground = true;
        }
    }
    void move(nta::crvec2 translation) {
        m_cen += translation;
        while (abs(m_cen.x) > 100) m_cen.x -= glm::sign(m_cen.x)*200;
    }
};

class GameState {
private:
    /// Using ECS is overkill here, but meh
    nta::ECS m_ecs;
    nta::Entity m_player;
    nta::ComponentID m_player_ball_component_id;
    int m_num_objects = 10;
    int m_num_active_screens = 2;
public:
    GameState();
    bool is_over() const {
        return m_num_active_screens <= 0;
    }
    void draw_player(nta::SpriteBatch& batch) const {
        // get_component returns an nta::Option.
        // Using map means we only try and draw the BallComponent when it is succesfully found
        m_ecs.get_component<BallComponent>(m_player).map([&batch](BallComponent& cmpn) {
            cmpn.draw(batch);
        });
    }
    void draw_objects(nta::SpriteBatch& batch) const {
        for (BallComponent* cmpn : m_ecs.get_component_list<BallComponent>()) {
            if (cmpn->get_id() != m_player_ball_component_id) {
                cmpn->draw(batch);
            }
        }
    }
    void move_left() {
        // If you want, instead of using map you can just do something like
        // BallComponent& cmpn = m_ecs.get_component<BallComponent>(m_player).unwrap();
        // but in that case you should be confident that m_player actually has a BallComponent
        m_ecs.get_component<BallComponent>(m_player).map([](BallComponent& cmpn) {
            cmpn.move(glm::vec2(-1, 0));
        });
    }
    void move_right() {
        m_ecs.get_component<BallComponent>(m_player).map([](BallComponent& cmpn) {
            cmpn.move(glm::vec2(1, 0));
        });
    }
    void update(float dt) {
        for (BallComponent* cmpn : m_ecs.get_component_list<BallComponent>()) {
            cmpn->update(dt);
        }
    }
    void closed_screen() {
        m_num_active_screens--;
    }
};

GameState::GameState() {
    m_player = m_ecs.gen_entity();
    m_player_ball_component_id = m_ecs.add_component<BallComponent>(m_player, glm::vec2(0, -90), 5, glm::vec4(1,0,0,1));
    for (int i = 0; i < m_num_objects; i++) {
        auto x_pos = nta::Random::randFloat(-100, 100);
        auto rad = nta::Random::randGaussian(5, 2);
        auto col = nta::Random::randRGB();
        m_ecs.add_component<BallComponent>(m_ecs.gen_entity(), glm::vec2(x_pos, 90),
                                           rad,  glm::vec4(col, 1));
    }
}

#endif // GAMESTATE_H_INCLUDED