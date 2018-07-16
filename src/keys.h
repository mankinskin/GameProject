#pragma once
#include "contextwindow.h"
#include "signal.h"
#include "signal.h"
#include "signal.h"

namespace input
{
    struct KeyEvent
    {
        KeyEvent()
            : key(-1)
            , change(-1)
        {}
        KeyEvent(int pKey, int pAction);

        int key;
        int change;
    };

    inline bool operator==(KeyEvent const & l, KeyEvent const& r)
    {
        return l.key == r.key && l.change == r.change;
    }
    struct KeySignal
    {
        KeySignal()
        {}
        KeySignal(int pKey);
        signals::Listener::ID press;
        signals::Listener::ID release;
    };

    extern KeySignal key_esc;
    extern KeySignal key_c;
    extern KeySignal key_g;
    extern KeySignal key_h;
    extern KeySignal key_i;
    extern KeySignal key_w;
    extern KeySignal key_s;
    extern KeySignal key_a;
    extern KeySignal key_d;
    extern KeySignal key_space;
    extern KeySignal key_z;
    extern KeySignal key_f;
    extern KeySignal key_n;
    extern KeySignal key_j;
    extern KeySignal key_up;
    extern KeySignal key_down;
    extern KeySignal key_left;
    extern KeySignal key_right;
    extern KeySignal key_o;
    extern KeySignal key_l;
    extern KeySignal key_lshift;
    extern KeySignal key_x;

    void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
    void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
}
