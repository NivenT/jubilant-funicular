#ifndef PLAYERSCREEN_H_INCLUDED
#define PLAYERSCREEN_H_INCLUDED

#include <nta/Screen.h>
#include <nta/Logger.h>
#include <nta/ResourceManager.h>

#include "GameState.h"

class PlayerScreen : public nta::Screen {
private:
    nta::GLSLProgram* m_glsl_prog = nullptr;
    nta::SpriteFont* m_font;
    nta::SpriteBatch m_batch;
    nta::Camera2D m_camera;
    GameState * const m_state;
public:
    PlayerScreen(GameState* state) : m_state(state) {}
    void init();
    void update();
    void render();
    /// Called whenever the window switches screens (e.g. when the window closes)
    void offFocus() {
        m_state->closed_screen();
    }
};

void PlayerScreen::init() {
    nta::Logger::writeToLog("Initializing PlayerScreen...");

    m_glsl_prog = m_manager->getContextData().getGLSLProgram("simple2D");
    if (!m_glsl_prog->isLinked()) {
        m_glsl_prog->addAttribute("pos");
        m_glsl_prog->addAttribute("color");
        m_glsl_prog->addAttribute("uv");
        m_glsl_prog->linkShaders();
    }

    auto maybe_font = m_manager->getContextData().getSpriteFont("font.otf");
    assert(maybe_font.is_ok()); // nothing should go wrong
    m_font = maybe_font.unwrap();
    
    m_batch.init();

    nta::Logger::writeToLog("PlayerScreen initialized");
}

void PlayerScreen::update() {
    if (m_state->is_over()) {
        close();
    }
    if (getInput().isPressed(SDLK_a)) {
        m_state->move_left();
    }
    if (getInput().isPressed(SDLK_d)) {
        m_state->move_right();
    }
}

void PlayerScreen::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        m_state->draw_player(m_batch, m_manager->getContextData());
        if (m_state->lost()) m_state->draw_winner(m_batch, m_manager->getContextData());

        std::string text = "FPS: " + nta::utils::to_string((int)m_manager->getFPS());
        m_font->drawText(m_batch, text, glm::vec4(-100, 100, 20, 10));
    } m_batch.end();

    m_glsl_prog->use(); {
        glUniformMatrix3fv(m_glsl_prog->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_batch.render();
    } m_glsl_prog->unuse();

    m_window->swapBuffers();
}

#endif // PLAYERSCREEN_H_INCLUDED