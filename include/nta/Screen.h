#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "nta/Window.h"

namespace nta {
    class ScreenManager;
    enum class ScreenState{NONE, RUNNING, SWITCH, SWITCH_ESC, SWITCH_X};
    /// Key unlocking the setManager() "private" function of class Screen
    class SetManagerKey {
        friend class ScreenManager;
        SetManagerKey() {}
        SetManagerKey(const SetManagerKey&);
        SetManagerKey& operator=(const SetManagerKey&);
    };
    /// Key unlocking the setIndices() "private" function of class Screen
    class SetIndicesKey {
        friend class ScreenManager;
        SetIndicesKey() {}
        SetIndicesKey(const SetIndicesKey&);
        SetIndicesKey& operator=(const SetIndicesKey&);
    };
    /// Key unlocking the setWindow() "private" function of class Screen
    class SetWindowKey {
        friend class ScreenManager;
        SetWindowKey() {}
        SetWindowKey(const SetWindowKey&);
        SetWindowKey& operator=(const SetWindowKey&);
    };
    /// Represents a game screen
    class Screen {
    private:
        /// the index of this screen in its container
        int m_index = -1;
        /// the index of the screen to go to when the Esc key is pressed
        int m_escIndex = -1;
        /// the index of the screen to go to when the window's X button is pressed
        int m_xIndex = -1;
    protected:
        /// the state of this screen
        ScreenState m_state = ScreenState::NONE;
        /// the window the screen is rendered in
        Window* m_window = nullptr;
        /// the ScreenManager that owns this screen
        ScreenManager* m_manager = nullptr;
        /// the name of the Screen
        std::string m_name;
        /// the index of the screen to go to in special circumstances
        int m_nextIndex = -1;
    public:
        /// basic constructor and destructor
        Screen(crstring name = "nameless");
        virtual ~Screen();
        /// returns state of screen
        ScreenState getState() const;
        /// gets name of screen
        std::string getName() const;
        /// gets various screen indices
        int getEscIndex() const;
        int getXIndex() const;
        int getNextIndex() const;
        int getIndex() const;
        /// Sets the manager of this screen
        void setManager(ScreenManager* manager, SetManagerKey key);
        /// sets various screen indices
        void setIndices(int index, int escIndex, int xIndex, SetIndicesKey key);
        /// sets the window to associate with this screen
        void setWindow(crstring title, SetWindowKey key);
        /// renders screen
        virtual void render() = 0;
        /// updates screen
        virtual void update() = 0;
        /// handles user input
        virtual void handleInput();
        /// called when the screen becomes active
        virtual void onFocus();
        /// called when the screen is no longer active
        virtual void offFocus();
        /// initializes the screen
        virtual void init() = 0;
    };
}

#endif // SCREEN_H_INCLUDED
