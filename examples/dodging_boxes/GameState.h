#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <mutex>

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
    BallComponent(nta::crvec2 cen, float rad, nta::crvec4 col) : m_col(col), m_rad(rad), m_cen(cen), m_vel(0) {}
    bool has_touched_ground() const { return m_touched_ground; }
    void draw(nta::SpriteBatch& batch, nta::ContextData& context) const {
        // Using Result::map means that nothing will be rendered if an error occurs
        // while trying to get the texture
        context.getTexture("circle.png").map([&](const nta::GLTexture& tex) {
            batch.addGlyph(glm::vec4(m_cen.x-m_rad, m_cen.y+m_rad, m_rad*2.f, m_rad*2.f),
                           glm::vec4(0, 0, 1, 1), tex.id, m_col);
        });
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
    bool collide(const BallComponent& rhs) {
        auto diff = m_cen - rhs.m_cen;
        auto rad = m_rad + rhs.m_rad;
        return diff.x*diff.x + diff.y*diff.y < rad*rad;
    }
};

class GameState {
private:
    // I'm overly causious in my use of this here
    static std::mutex m_state_lock;

    /// Using ECS is overkill here, but meh
    nta::ECS m_ecs;
    nta::Entity m_player;
    nta::ComponentID m_player_ball_component_id;
    const int m_num_objects = 10;
    int m_num_active_screens = 2;
public:
    GameState();
    bool is_over() const {
        return m_num_active_screens <= 0;
    }
    void draw_player(nta::SpriteBatch& batch, nta::ContextData& context) const {
        std::lock_guard<std::mutex> g(m_state_lock);
        // get_component returns an nta::Option.
        // Using map means we only try and draw the BallComponent when it is succesfully found
        m_ecs.get_component<BallComponent>(m_player).map([&](BallComponent& cmpn) {
            cmpn.draw(batch, context);
        });
    }
    void draw_objects(nta::SpriteBatch& batch, nta::ContextData& context) const {
        std::lock_guard<std::mutex> g(m_state_lock);
        for (BallComponent* cmpn : m_ecs.get_component_list<BallComponent>()) {
            if (cmpn->get_id() != m_player_ball_component_id) {
                cmpn->draw(batch, context);
            }
        }
    }
    void move_left() {
        std::lock_guard<std::mutex> g(m_state_lock);
        // If you want, instead of using map you can just do something like
        // BallComponent& cmpn = m_ecs.get_component<BallComponent>(m_player).unwrap();
        // but in that case you should be confident that m_player actually has a BallComponent
        m_ecs.get_component<BallComponent>(m_player).map([](BallComponent& cmpn) {
            cmpn.move(glm::vec2(-1, 0));
        });
    }
    void move_right() {
        std::lock_guard<std::mutex> g(m_state_lock);
        m_ecs.get_component<BallComponent>(m_player).map([](BallComponent& cmpn) {
            cmpn.move(glm::vec2(1, 0));
        });
    }
    void update(float dt) {
        std::lock_guard<std::mutex> g(m_state_lock);
        if (m_ecs.num_entities() < m_num_objects && nta::Random::randFloat() < dt*2.f) {
            auto x_pos = nta::Random::randFloat(-100, 100);
            auto rad = nta::Random::randGaussian(6, 3);
            auto col = nta::Random::randRGB();
            m_ecs.add_component<BallComponent>(m_ecs.gen_entity(), glm::vec2(x_pos, 90),
                                               rad,  glm::vec4(col, 1));
        }

        std::vector<BallComponent*> trash;
        for (BallComponent* cmpn : m_ecs.get_component_list<BallComponent>()) {
            cmpn->update(dt);
            if (cmpn->get_id() != m_player_ball_component_id) {
                nta::utils::Option<BallComponent&> pball = m_ecs.get_component<BallComponent>(m_player);
                if (pball.is_some() && cmpn->collide(pball.unwrap())) {
                    m_num_active_screens = 0;
                } else if (cmpn->has_touched_ground()) {
                    trash.push_back(cmpn);
                }
            }
        }
        for (auto& cmpn : trash) {
            m_ecs.delete_entity(m_ecs.get_owner(cmpn->get_id()));
        }
    }
    void closed_screen() {
        m_num_active_screens--;
    }
};
std::mutex GameState::m_state_lock;

GameState::GameState() {
    m_player = m_ecs.gen_entity();
    m_player_ball_component_id = m_ecs.add_component<BallComponent>(m_player, glm::vec2(0, -90), 5, glm::vec4(1,0,0,1));
}

#endif // GAMESTATE_H_INCLUDED