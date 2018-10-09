/*
 * Here's an example of a program with multiple windows. Each window draws a
 * single red square. Using WASD, you can independently move around the squares
 * in each of the windows. See dodging_boxes for a more involved example with
 * multiple windows
 */
#include <iostream>
#include <thread>

#include <nta/ScreenManager.h>
#include <nta/InputManager.h>
#include <nta/SpriteBatch.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Logger.h>

using namespace nta;
using namespace std;
using namespace glm;

class SquareScreen : public Screen {
private:
    GLSLProgram* m_simple_prog = nullptr;
    SpriteBatch m_batch;
    Camera2D m_camera;

    vec2 top_left;
public:
    SquareScreen() : top_left(-25, 25) {}
    ~SquareScreen() {}
    void init();
    void update();
    void render();
};

void SquareScreen::init() {
    Logger::writeToLog("Initializing screen...");

    m_simple_prog = m_manager->getContextData().getGLSLProgram("simple2D");
    if (!m_simple_prog->isLinked()) {
        m_simple_prog->addAttribute("pos");
        m_simple_prog->addAttribute("color");
        m_simple_prog->addAttribute("uv");
        m_simple_prog->addAttribute("hasTexture");
        m_simple_prog->linkShaders();
    }

    m_batch.init();

    Logger::writeToLog("screen initialized");
}

void SquareScreen::update() {
    if (getInput().isPressed(SDLK_w)) {
        top_left.y += 1;
    }
    if (getInput().isPressed(SDLK_a)) {
        top_left.x -= 1;
    }
    if (getInput().isPressed(SDLK_s)) {
        top_left.y -= 1;
    }
    if (getInput().isPressed(SDLK_d)) {
        top_left.x += 1;
    }

    if (abs(top_left.x) > 100 || abs(top_left.y) > 100) {
        // Use nta::{lock_stream, unlock_stream} to make printing thread_safe
        // Technically unnecessary here because only one window will have a moving
        // square at any time, but its still something good to know about.
        cout<<nta::lock_stream
            <<"The square in "<<m_window->getTitle()<<" just wrapped around."<<std::endl
            <<nta::unlock_stream;
    }
    
    while (abs(top_left.x) > 100) top_left.x -= sign(top_left.x)*200;
    while (abs(top_left.y) > 100) top_left.y -= sign(top_left.y)*200;
}

void SquareScreen::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        m_batch.addGlyph(vec4(top_left, 50, 50), vec4(0), 0, vec4(1,0,0,1));
    } m_batch.end();

    m_simple_prog->use(); {
        glUniformMatrix3fv(m_simple_prog->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_batch.render();
    } m_simple_prog->unuse();

    m_window->swapBuffers();
}

int main(int argc, char* argv[]) {
    // Remember to initialize the library
    nta::init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Each Window is ownder by a ScreenManager
    // In order for the windows to be able to run at the same time, we give each
    // one its own thread.
    thread t1 = thread([]{
        ScreenManager s1("Window 1", 60);
        s1.addScreen(new SquareScreen);
        s1.run();
        s1.destroy();
    });
    
    thread t2 = thread([]{
        ScreenManager s2("Window 2", 60);
        s2.addScreen(new SquareScreen);
        s2.run();
        s2.destroy();
    });
    
    // wait for the threads to finish
    t1.join();
    t2.join();

    cleanup();
    Logger::writeToLog("Program exited cleanly");

    return 0;
}
