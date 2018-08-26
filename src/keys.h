#pragma once
#include "contextwindow.h"
#include "buttonsignals.h"

namespace input
{
    typedef int Key;
    typedef signals::ButtonSignals<Key, int> KeySignals;

    extern utils::ID<KeySignals> key_esc;
    extern utils::ID<KeySignals> key_c;
    extern utils::ID<KeySignals> key_g;
    extern utils::ID<KeySignals> key_h;
    extern utils::ID<KeySignals> key_i;
    extern utils::ID<KeySignals> key_w;
    extern utils::ID<KeySignals> key_s;
    extern utils::ID<KeySignals> key_a;
    extern utils::ID<KeySignals> key_d;
    extern utils::ID<KeySignals> key_space;
    extern utils::ID<KeySignals> key_z;
    extern utils::ID<KeySignals> key_f;
    extern utils::ID<KeySignals> key_n;
    extern utils::ID<KeySignals> key_j;
    extern utils::ID<KeySignals> key_up;
    extern utils::ID<KeySignals> key_down;
    extern utils::ID<KeySignals> key_left;
    extern utils::ID<KeySignals> key_right;
    extern utils::ID<KeySignals> key_o;
    extern utils::ID<KeySignals> key_l;
    extern utils::ID<KeySignals> key_lshift;
    extern utils::ID<KeySignals> key_x;

    void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
    void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
}
