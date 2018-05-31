#pragma once
#include "keys.h"
#include "gates.h"
#include "event.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>

namespace input 
{
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
    extern events::ButtonEvents<events::Event> lmb;
    extern events::ButtonEvents<events::Event> rmb;
    extern unsigned int always;
    void init();
    void setupControls();
    void fetchGLFWEvents();
    void end();
}
