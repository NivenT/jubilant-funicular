/*
 * Here's an example of rendering a square to the screen using this library
 * Admitedly, there is a lot of boilerplate involved for such a simple task
 * but this extra required structure is helpful for larger projects.
 */
#include <nta/ScreenManager.h>
#include <nta/SystemManager.h>
#include <nta/InputManager.h>
#include <nta/SpriteBatch.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Logger.h>

using namespace std;
using namespace glm;

class SquareScreen : public nta::Screen {
private:
    // Stores the GLSL code used during rendering
    // the code will be loaded from an external file
    nta::GLSLProgram* m_simple_prog = nullptr;
    // A "batch" (collection) of "sprites" (possibly textured rectangles)
    // Rendering happens by first collecting all sprites in a batch, and then
    //   drawing the batch all in one go
    nta::SpriteBatch m_batch;
    // Defines the camera/coordinate system used when rendering
    // by default the center of the screen is (0,0)
    //            the top right is            (100,100)
    //            the bottom left is          (-100,-100)
    nta::Camera2D m_camera;
public:
    SquareScreen() {}
    ~SquareScreen() {}
    // The following three functions must be defined children of the Screen class
    void init();
    void update();
    void render();
};

void SquareScreen::init() {
    nta::Logger::writeToLog("Initializing main screen...");

    // Admitedlly, the simple2D I'm using is more complicated than need be
    // Loads a GLSLProgram (vertex and fragment shader) from a pair of files
    //   named "simple2D.vert" and "simple2D.frag"
    m_simple_prog = nta::SystemManager::getGLSLProgram("simple2D");
    if (!m_simple_prog->isLinked()) {
        // Adds the various attributes used by simple2D.vert
        // the order here must match the order of the fields of Vertex2D
        m_simple_prog->addAttribute("pos");
        m_simple_prog->addAttribute("color");
        m_simple_prog->addAttribute("uv");
        m_simple_prog->addAttribute("hasTexture");
        m_simple_prog->linkShaders();
    }
    m_simple_prog->use();
    // bind GL_TEXTURE0 by default
    glUniform1i(m_simple_prog->getUniformLocation("sampler"), 0);
    m_simple_prog->unuse();

    m_batch.init();

    nta::Logger::writeToLog("main screen initialized");
}

void SquareScreen::update() {
    // If the enter key was just pressed...
    if (nta::InputManager::justPressed(SDLK_RETURN)) {
        // Let the ScreenManager know that it is time to switch screens
        // Switch to the screen at index -1 (i.e. exit the ScreenManager)
        m_state = nta::ScreenState::SWITCH;
        m_nextIndex = -1;
    }
}

void SquareScreen::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Collect all the glyphs to be drawn
    m_batch.begin();
    // Draws a square whose topleft corner is at (-25, 25) and whose
    // width and height are both 50. The uv (i.e. texture) coordinates
    // used are given by a square whose topleft corner is at (0, 0) and
    // whose width and height are both 0. This is an empty square which
    // would be problematic except that the texture id suuplied here is
    // 0, so this glyph won't actually use a texture at all (i.e. since
    // the third parameter is 0, the second one is irrelevant). Finally,
    // this square is colored red: (1,0,0,1) in RGBA.
    m_batch.addGlyph(vec4(-25, 25, 50, 50), vec4(0), 0, vec4(1,0,0,1));
    m_batch.end();

    // Actually draw stuff
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

    // A ScreenManager let's you easily switch between different Screens
    //   and it handles capping the FPS and things like that
    // This creates a ScreenManager attached to a window title "Basic Square"
    //   with a cap of 60fps
    nta::ScreenManager screenManager("Basic Square", 60);
    // This is when SquareScreen::init() is called
    screenManager.addScreen(new SquareScreen);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Executes the whole input, update, render loop
    screenManager.run(); 
    
    // Delete all Screens, freeing any memory necessary
    screenManager.destroy();
    // Safely destoryes all static classes used by the library
    nta::cleanup();
    nta::Logger::writeToLog("Program exited cleanly");

    return 0;
}
