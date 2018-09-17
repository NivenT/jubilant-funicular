/*
 * Here's an example of using the ECS class to manage a bunch of entities (e.g. balls)
 * Click on the screen to add a ball.
 * The program exists when there are no balls left
 *
 * The physics in this example is less than ideal since it is not the focus
 * In practice, for decent physics, you'll want to use an externel library
 */

#include <nta/ScreenManager.h>
#include <nta/SystemManager.h>
#include <nta/ResourceManager.h>
#include <nta/InputManager.h>
#include <nta/SpriteBatch.h>
#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/ECS.h>
#include <nta/Random.h>
#include <nta/Logger.h>

#define NUM_INIT_BALLS 20
#define BALL_DENSITY 0.3f
#define COLLISION_DECAY_MIN 0.6f
#define COLLISION_DECAY_MAX 1.0f
#define GRAVITY vec2(0, -9.81)
#define GROUND_Y -100.f

using namespace std;
using namespace glm;

class GraphicsComponent : public nta::Component {
private:
    vec4 m_color;
    vec2 m_pos;
    float m_rad;
public:
    GraphicsComponent() : m_color(1), m_pos(0), m_rad(5) {}
    GraphicsComponent(float rad) : m_color(1), m_pos(0), m_rad(rad) {}
    GraphicsComponent(vec4 col, float rad) : m_color(col),  m_pos(0), m_rad(rad) {}

    vec2 get_pos() const { return m_pos; }
    void draw(nta::SpriteBatch& batch) {
        // Gets a cached texture so we don't repeatedly load in the same file
        nta::Result<nta::GLTexture> tex = nta::ResourceManager::getTexture("circle.png");
        // There should be no error
        assert(!tex.is_err());
        // get the id for the associated gl texture
        GLuint id = tex.get_data().id;

        vec2 top_left = m_pos + vec2(-m_rad, m_rad);
        batch.addGlyph(vec4(top_left, 2.f*m_rad, 2.f*m_rad), vec4(0,0,1,1), id, m_color);
    }
    void receive(const nta::Message& message) {
        // there's also a message.type field
        // This program only uses one type of message so that field is irrelevant
        m_pos = *(vec2*)message.data;
    }
};

class PhysicsComponent : public nta::Component {
private:
    vec2 m_pos;
    vec2 m_vel;
    vec2 m_acc;
    float m_rad;
public:
    PhysicsComponent() {
        m_pos = m_vel = m_acc = vec2(0);
        m_rad = 5;
    }
    PhysicsComponent(nta::crvec2 pos, float radius) : m_pos(pos), m_rad(radius) {
        m_vel = m_acc = vec2(0);
    }

    float get_mass() const { return m_rad*m_rad*BALL_DENSITY; }
    vec2 get_pos() const { return m_pos; }
    void step(float dt) {
        vec2 acc = m_acc + GRAVITY;
        m_vel += acc * dt;
        m_pos += m_vel * dt;

        if (m_pos.y - m_rad < GROUND_Y) {
            m_vel.y = abs(m_vel.y);
        }

        send(nta::Message(0, &m_pos));
    }
    // Reminder: Ignore the physics in this example
    // It's not the focus and it is wrong
    void collide(PhysicsComponent& other, float dt) {
        vec2 diff = other.m_pos - m_pos;
        float dist2 = diff.x*diff.x + diff.y*diff.y;

        float rad_sum = m_rad + other.m_rad;
        if (dist2 < rad_sum * rad_sum) {
            float tot_mass = get_mass() + other.get_mass();
            float mass_diff = get_mass() - other.get_mass();
            vec2 vel_diff = m_vel - other.m_vel;

            vec2 v1 = 2*other.get_mass()*dot(vel_diff, diff)*diff;
            v1 = m_vel - v1/(tot_mass*dist2);

            vec2 v2 = 2*get_mass()*dot(vel_diff, diff)*diff;
            v2 = other.m_vel + v2/(tot_mass*dist2);

            float decay = nta::Random::randFloat(COLLISION_DECAY_MIN, COLLISION_DECAY_MAX);
            m_vel = v1 * decay;
            other.m_vel = v2 * decay;

            float overlap = rad_sum - sqrt(dist2);
            m_pos -= diff*overlap/rad_sum;
        }
    }
    // Every Component needs to be able to receive Messages
    void receive(const nta::Message& _) {}
};

class MainScreen : public nta::Screen {
private:
    // Gets the position of the mouse in camera (or world or whatever you wanna call it) coordinates
    vec2 getMouse() const;
    void addBall(nta::crvec2 pos);

    nta::GLSLProgram* m_simple_prog = nullptr;
    nta::SpriteFont* m_font;
    nta::SpriteBatch m_batch;
    nta::Camera2D m_camera;

    vector<nta::Entity> m_balls;
    // An Entity Component System
    nta::ECS m_system;
public:
    MainScreen() {}
    ~MainScreen() {}
    void init();
    void update();
    void render();
};

vec2 MainScreen::getMouse() const {
    // width by height of window
    vec2 win_dims = m_window->getDimensions();
    vec2 screen = nta::InputManager::getMouseCoordsStandard(win_dims[1]);
    // convert screen to have coordinates in the range [-1, 1] like gl_Position
    screen *= 2.f/win_dims;
    screen -= 1.f;
    // essentially multiply by inverse camera matrix
    return m_camera.screenToGame(screen);
}

void MainScreen::addBall(nta::crvec2 pos) {
    // Generate a new entity (i.e. ball)
    m_balls.emplace_back(m_system.gen_entity());

    float rad = nta::Random::randFloat(3.f, 10.f);
    vec4 col(nta::Random::randRGB(), 1.f);

    // Give the ball graphics and physics
    m_system.add_component<PhysicsComponent>(PhysicsComponent(pos, rad), m_balls.back());
    m_system.add_component<GraphicsComponent>(GraphicsComponent(col, rad), m_balls.back());

    // Deleted Entites have their IDs recycled. Look at Log.log to see what I mean
    nta::Logger::writeToLog("Added ball with id " + nta::utils::to_string(m_balls.back()));
}

void MainScreen::init() {
    nta::Logger::writeToLog("Initializing main screen...");

    m_simple_prog = nta::SystemManager::getGLSLProgram("simple2D");
    if (!m_simple_prog->isLinked()) {
        m_simple_prog->addAttribute("pos");
        m_simple_prog->addAttribute("color");
        m_simple_prog->addAttribute("uv");
        m_simple_prog->linkShaders();
    }
    m_simple_prog->use();
    // bind GL_TEXTURE0 by default
    glUniform1i(m_simple_prog->getUniformLocation("sampler"), 0);
    m_simple_prog->unuse();

    m_batch.init();
    m_font = nta::ResourceManager::getSpriteFont("font.otf");

    // Here we initialize a bunch of random balls
    for (int i = 0; i < NUM_INIT_BALLS; i++) {
        addBall(nta::Random::randVec2(-100.f, 100.f));
    }
    nta::Logger::writeToLog("main screen initialized");
}

void MainScreen::update() {
    // Adds a ball whenver you click on the screen
    if (nta::InputManager::justPressed(SDL_BUTTON_LEFT)) {
        addBall(getMouse());
    }

    vector<PhysicsComponent*> phys_cpmns = m_system.get_component_list<PhysicsComponent>();
    for (int i = 0; i < phys_cpmns.size(); i++) {
        for (int j = i+1; j < phys_cpmns.size(); j++) {
            phys_cpmns[i]->collide(*phys_cpmns[j], 1./60);
        }
    }
    for (PhysicsComponent* cmpn : phys_cpmns) {
        cmpn->step(1./60);
    }

    // removes any ball whose center goes off the screen
    for (int i = 0; i < m_balls.size(); i++) {
        auto& ball = m_balls[i];
        // This has type Option<PhysicsComponent> (its none if an invalid entity is supplied)
        auto phys = m_system.get_component<PhysicsComponent>(ball);
        // nothing should go wrong
        assert(phys.is_some());
        // Get the ball's position from its physics component
        vec2 pos = phys.unwrap().get_pos();
        if (pos.x > 100 || pos.x < -100 || pos.y > 100 || pos.y < -100) {
            m_system.delete_entity(ball);
            // ordering in m_balls doesn't matter so might as well
            // do an O(1) deletion
            swap(ball, m_balls.back());
            m_balls.pop_back();
            i--;
        }
    }
    // Note: there are better ways to do the above
    // * one would be to make PhysicsComponent an nta::Subject
    // * and make MainScreen an nta::Observer. Then, the component could signal
    // * MainScreen of a ball going out of bounds during the call to step.
    // * MainScreen could keep track of all the balls then went out of bounds
    // * and delete them all at the end of the update method

    if (m_balls.size() == 0) {
        // Let the ScreenManager know that it is time to switch screens
        // Switch to the screen at index -1 (i.e. exit the ScreenManager)
        m_state = nta::ScreenState::SWITCH;
        m_nextIndex = -1;
    }
}

void MainScreen::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        for (GraphicsComponent* cmpn : m_system.get_component_list<GraphicsComponent>()) {
            cmpn->draw(m_batch);
        }

        string text = "FPS: " + nta::utils::to_string((int)m_manager->getFPS());
        m_font->drawText(m_batch, text, vec4(-100, 100, 20, 10));
    } m_batch.end();

    m_simple_prog->use(); {
        glUniformMatrix3fv(m_simple_prog->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_batch.render();
    } m_simple_prog->unuse();

    m_window->swapBuffers();
}

int main(int argc, char* argv[]) {
    nta::init();

    nta::ScreenManager screenManager("\"Bouncing\" Balls", 60);
    screenManager.addScreen(new MainScreen);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(); 
    
    screenManager.destroy();
    nta::cleanup();
    nta::Logger::writeToLog("Program exited cleanly");

    return 0;
}