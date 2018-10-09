/*
 * Here's an example of a game with multiple windows. The goal of the game
 * is to dodge a bunch of falling boxes. The catch is that the boxes only
 * appear in one window, and the player only shows up on the other.
 */
#include <iostream>
#include <thread>

#include <nta/ScreenManager.h>
#include <nta/InputManager.h>
#include <nta/SpriteBatch.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Logger.h>

#include "PlayerScreen.h"
#include "ObjectScreen.h"

using namespace nta;
using namespace std;
using namespace glm;

int main(int argc, char* argv[]) {
    // Remember to initialize the library
    nta::init();

    // Global GameState shared by both threads
    GameState state;
    // Each Window is ownder by a ScreenManager
    // In order for the windows to be able to run at the same time, we give each
    // one its own thread.
    thread t1 = thread([&state]{
        ScreenManager s1("Player Screen", 60);
        s1.addScreen(new PlayerScreen(&state));
        
        // These calls only make sense after an OpenGL context has been set up
        // i.e. after a Window was created by the ScreenManager constructor
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        s1.run();
        s1.destroy();
    });
    
    thread t2 = thread([&state]{
        ScreenManager s2("Block Screen", 60);
        s2.addScreen(new ObjectScreen(&state));
        
        // These calls only make sense after an OpenGL context has been set up
        // i.e. after a Window was created by the ScreenManager constructor
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        s2.run();
        s2.destroy();
    });

    // Here, the GameState is updated in its own thread
    thread t3 = thread([&state]{
        nta::FPSLimiter limiter(60);
        while (!state.is_over()) {
            limiter.begin();
            if (limiter.getFPS() >= 1) state.update(1./limiter.getFPS());
            limiter.end();
        }
    });
    
    // wait for the threads to finish
    t1.join();
    t2.join();
    t3.join();

    cleanup();
    Logger::writeToLog("Program exited cleanly");

    return 0;
}
