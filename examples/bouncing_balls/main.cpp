/*
 * Here's an example of using the ECS class to manage a bunch of entities (e.g. balls)
 * Click on the screen to add a ball.
 * The program exists when there are no balls left
 *
 * The physics in this example is less than ideal since it is not the focus
 * In practice, for decent physics, you'll want to use an external library
 */

#include <nta/ScreenManager.h>
#include <nta/ResourceManager.h>
#include <nta/SpriteBatch.h>
#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/ECS.h>
#include <nta/Random.h>
#include <nta/Logger.h>
#include <nta/format.h>

#define NUM_INIT_BALLS 20
#define BALL_DENSITY 0.3f
#define COLLISION_DECAY_MIN 0.6f
#define COLLISION_DECAY_MAX 1.0f
#define GRAVITY vec2(0, -9.81)
#define GROUND_Y -100.f

using namespace std;
using namespace glm;

class GraphicsComponent;
class PhysicsComponent;

// see event_tests.cpp for more information on how events work
//
// Briefly, in order to keep things type-safe, events are supplied
// with a list of possible function signatures indexed by a given
// enum type, and then their particular effects can be customized
// on an object-by-object basis
enum class ComponentType { GRAPHICS, PHYSICS, COUNT };
using Event = nta::EventTemplate<ComponentType, void(GraphicsComponent&), void(PhysicsComponent&)>;

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
    // nta::crvec2 is short hand for "const glm::vec2&"
    void set_pos(nta::crvec2 pos) { m_pos = pos; }
    // The ContextData is what stores GLTextures, among other things
    void draw(nta::SpriteBatch& batch, nta::ContextData& context) {
        // Gets a cached texture so we don't repeatedly load in the same file
        nta::Result<nta::GLTexture> tex = context.getTexture("circle.png");
        // There should be no error
        assert(!tex.is_err());
        // Get the id of this texture
        GLuint id = tex.get_data().id;

        vec2 top_left = m_pos + vec2(-m_rad, m_rad);
        batch.addGlyph(vec4(top_left, 2.f*m_rad, 2.f*m_rad), vec4(0,0,1,1), id, 
                       m_color);
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
    void step(nta::ECS& ecs, float dt) {
        vec2 acc = m_acc + GRAVITY;
        m_vel += acc * dt;
        m_pos += m_vel * dt;

        if (m_pos.y - m_rad < GROUND_Y) {
            m_vel.y = abs(m_vel.y);
        }

        // Create an event that updates the position of a GraphicsComponent
        Event update_pos;
        update_pos.define_for<ComponentType::GRAPHICS>([&](GraphicsComponent& gc) {
            gc.set_pos(m_pos);
        });
        // Enact this event on the GraphicsComponent with the same owner at this 
        ecs.enact_on_sibling<GraphicsComponent>(update_pos, m_id);
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
    MainScreen(const nta::ComponentRegistry& reg) : m_system(reg) {}
    ~MainScreen() {}
    void init();
    void update();
    void render();
};

vec2 MainScreen::getMouse() const {
    // width by height of window
    vec2 win_dims = m_window->getDimensions();
    vec2 screen = getInput().getMouseCoordsStandard(win_dims[1]);
    // convert screen to have coordinates in the range [-1, 1] like gl_Position
    screen *= 2.f/win_dims;
    screen -= 1.f;
    // essentially multiply by inverse camera matrix
    return m_camera.screenToGame(screen);
}

void MainScreen::addBall(nta::crvec2 pos) {
    // Generate a new entity (i.e. ball)
    m_balls.emplace_back(m_system.gen_entity());

    // Deleted Entities have their IDs recycled. Look at Log.log to see what I mean
    //
    // Their id is a generational index to prevent "use after free" errors
    // if a part of the code holds onto to one after that entity is deleted
    nta::Logger::writeToLog("Adding ball with id {}", m_balls.back());

    float rad = nta::Random::randFloat(3.f, 10.f);
    vec4 col(nta::Random::randRGB(), 1.f);

    // Give the ball graphics and physics
    m_system.add_component<PhysicsComponent>(m_balls.back(), pos, rad);
    m_system.add_component<GraphicsComponent>(m_balls.back(), col, rad);
}

void MainScreen::init() {
    nta::Logger::writeToLog("Initializing main screen...");

    m_simple_prog = m_manager->getContextData().getGLSLProgram("simple2D");
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
    
    auto maybe_font = m_manager->getContextData().getSpriteFont("font.otf");
    assert(maybe_font.is_ok()); // nothing should go wrong
    m_font = maybe_font.unwrap();

    // Here we initialize a bunch of random balls
    for (int i = 0; i < NUM_INIT_BALLS; i++) {
        addBall(nta::Random::randVec2(-100.f, 100.f));
    }
    nta::Logger::writeToLog("main screen initialized");
}

void MainScreen::update() {
    // Adds a ball whenever you click on the screen
    if (getInput().justPressed(SDL_BUTTON_LEFT)) {
        addBall(getMouse());
    }

    nta::utils::SlotMap<PhysicsComponent>& phys_cmpns = m_system.get_component_list<PhysicsComponent>();
    for (auto it = phys_cmpns.begin(); it != phys_cmpns.end(); ++it) {
        for (auto it2 = it + 1; it2 != phys_cmpns.end(); ++it2) {
            it->collide(*it2, 1./60);
        }
    }
    for (auto& cmpn : phys_cmpns) {
        cmpn.step(m_system, 1./60);
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
        m_system.for_each<GraphicsComponent>([&](GraphicsComponent& graphics) {
            graphics.draw(m_batch, m_manager->getContextData());
        });

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
    /// All logged information will be sent to std::cout as well
    nta::Logger::useSecondLog(std::cout);

    /// Using nta::ECS requires a pre-made list of all components in the project
    nta::ComponentRegistry registry;
    registry.register_component<GraphicsComponent>();
    registry.register_component<PhysicsComponent>();

    nta::ScreenManager screenManager("\"Bouncing\" Balls", 60);
    screenManager.addScreen(new MainScreen(registry));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(); 
    
    screenManager.destroy();
    nta::cleanup();
    nta::Logger::writeToLog("Program exited cleanly");

    return 0;
}