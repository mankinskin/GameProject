#include "keys.h"
#include "signal.h"
#include "signal.h"
#include "gates.h"

input::KeySignals input::key_esc;
input::KeySignals input::key_c;
input::KeySignals input::key_g;
input::KeySignals input::key_h;
input::KeySignals input::key_i;
input::KeySignals input::key_w;
input::KeySignals input::key_s;
input::KeySignals input::key_a;
input::KeySignals input::key_d;
input::KeySignals input::key_space;
input::KeySignals input::key_z;
input::KeySignals input::key_f;
input::KeySignals input::key_n;
input::KeySignals input::key_j;
input::KeySignals input::key_up;
input::KeySignals input::key_down;
input::KeySignals input::key_left;
input::KeySignals input::key_right;
input::KeySignals input::key_o;
input::KeySignals input::key_l;
input::KeySignals input::key_lshift;
input::KeySignals input::key_x;


void input::key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods)
{
    signals::pushEvent(signals::Event<Key, int>((Key)pKey, pAction));
}

void input::char_Callback(GLFWwindow* window, unsigned int pCodepoint)
{
    //printf("char callBack! Char: %c\n", pCodepoint);
}
