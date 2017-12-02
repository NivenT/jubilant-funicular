#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "Window.h"

namespace nta {
    enum class ScreenState{NONE, RUNNING, SWITCH, SWITCH_ESC, SWITCH_X};
    ///Represents a game screen
    class Screen {
    private:
        ///the index of this screen in its container
        int                         m_index = -1;
        ///the index of the screen to go to when the Esc key is pressed
        int                         m_escIndex = -1;
        ///the index of the screen to go to when the window's X button is pressed
        int                         m_xIndex = -1;
    protected:
        ///the state of this screen
        ScreenState                 m_state = ScreenState::NONE;
        ///the window the screen is rendered in
        Window*                     m_window = nullptr;
        ///the index of the screen to go to in special circumstances
        int                         m_nextIndex = -1;
    public:
        ///basic constructor and destructor
                                    Screen();
        virtual                     ~Screen();
        ///returns state of screen
        ScreenState                 getState() const;
        ///sets/gets various screen indices
        virtual int                 getEscIndex() const;
        virtual int                 getXIndex() const;
        virtual int                 getNextIndex() const;
        virtual int                 getIndex() const;
        virtual void                setIndices(int index, int escIndex, int xIndex);
        ///sets the window to associate with this screen
        virtual void                setWindow(crstring title);
        ///renders screen
        virtual void                render() = 0;
        ///updates screen
        virtual void                update() = 0;
        ///handles user input
        virtual void                handleInput();
        ///called when the screen becomes active
        virtual void                onFocus();
        ///called when the screen is no longer active
        virtual void                offFocus();
        ///initializes the screen
        virtual void                init() = 0;
    };
}

#endif // SCREEN_H_INCLUDED
