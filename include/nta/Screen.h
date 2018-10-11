#ifndef NTA_SCREEN_H_INCLUDED
#define NTA_SCREEN_H_INCLUDED

#include "nta/Window.h"
#include "nta/InputManager.h"

namespace nta {
    class ScreenManager;
    class ResourceManager;
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
    /// Info passed to Screen::onFocus
    struct ScreenSwitchInfo {
        ScreenSwitchInfo() : data(nullptr), fromIndex(-1) {
        }
        ScreenSwitchInfo(void* data) : data(data), fromIndex(-1) {
        }
        ScreenSwitchInfo(void* data, int index) : data(data), fromIndex(index) {
        }

        /// arbitrary data to pass along
        void* data;
        /// the index of the screen switched from
        int fromIndex;
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
        const InputManager& getInput() const;

        /// the state of this screen
        ScreenState m_state = ScreenState::NONE;
        /// the ScreenManager that owns this screen
        ScreenManager* m_manager = nullptr;
        /// the Window this Screen appears on
        Window* m_window = nullptr;
        /// the name of the Screen
        std::string m_name;
        /// the index of the screen to go to when m_state == SWITCH
        int m_nextIndex = -1;
        /// data to pass to the new screen (via onFocus) when m_state == SWITCH
        void* m_switchData = nullptr;
    public:
        /// basic constructor and destructor
        Screen(crstring name = "UNAMED_SCREEN");
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
        /// gets the switch data (shoudl this require a key?)
        void* getSwitchData() const;
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
        /// called when the screen becomes active
        virtual void onFocus(const ScreenSwitchInfo& info);
        /// called when the screen is no longer active
        virtual void offFocus();
        /// initializes the screen
        virtual void init() = 0;
        void close() { m_state = ScreenState::SWITCH_X; }
        void esc() { m_state = ScreenState::SWITCH_ESC; }
    };
}

#endif // NTA_SCREEN_H_INCLUDED
