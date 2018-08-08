#pragma once
#include "contextwindow.h"
#include "signal.h"

namespace input
{
    typedef int Key;
    typedef signals::ButtonSignals<Key, int> KeySignals;

    extern KeySignals key_esc;
    extern KeySignals key_c;
    extern KeySignals key_g;
    extern KeySignals key_h;
    extern KeySignals key_i;
    extern KeySignals key_w;
    extern KeySignals key_s;
    extern KeySignals key_a;
    extern KeySignals key_d;
    extern KeySignals key_space;
    extern KeySignals key_z;
    extern KeySignals key_f;
    extern KeySignals key_n;
    extern KeySignals key_j;
    extern KeySignals key_up;
    extern KeySignals key_down;
    extern KeySignals key_left;
    extern KeySignals key_right;
    extern KeySignals key_o;
    extern KeySignals key_l;
    extern KeySignals key_lshift;
    extern KeySignals key_x;

    void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
    void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
}
