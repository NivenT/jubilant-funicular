#include "nta/Screen.h"
#include "nta/InputManager.h"
#include "nta/WindowManager.h"
#include "nta/ScreenManager.h"
#include "nta/ResourceManager.h"
#include "nta/Logger.h"

namespace nta {
    Screen::Screen(crstring name) : m_name(name) {
    }
    Screen::~Screen() {
    }
    const InputManager& Screen::getInput() const {
        return m_manager->getInput();
    }
    ScreenState Screen::getState() const {
        return m_state;
    }
    std::string Screen::getName() const {
        return m_name;
    }
    int Screen::getEscIndex() const {
        return m_escIndex;
    }
    int Screen::getXIndex() const {
        return m_xIndex;
    }
    int Screen::getIndex() const {
        return m_index;
    }
    int Screen::getNextIndex() const {
        return m_nextIndex;
    }
    void* Screen::getSwitchData() const {
        return m_switchData;
    }
    void Screen::setManager(ScreenManager* manager, SetManagerKey key) {
        m_manager = manager;
    }
    void Screen::setIndices(int index, int escIndex, int xIndex, SetIndicesKey key) {
        m_index = index;
        m_escIndex = escIndex;
        m_xIndex = xIndex;
    }
    void Screen::setWindow(crstring title, SetWindowKey key) {
        m_window = WindowManager::getWindow(title);
    }
    void Screen::onFocus(const ScreenSwitchInfo& _) {
        m_state = ScreenState::RUNNING;
    }
    void Screen::offFocus() {
    }
}
