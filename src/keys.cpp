#include "keys.h"
#include "signal.h"

utils::ID<input::KeySignals> input::key_esc;
utils::ID<input::KeySignals> input::key_c;
utils::ID<input::KeySignals> input::key_g;
utils::ID<input::KeySignals> input::key_h;
utils::ID<input::KeySignals> input::key_i;
utils::ID<input::KeySignals> input::key_w;
utils::ID<input::KeySignals> input::key_s;
utils::ID<input::KeySignals> input::key_a;
utils::ID<input::KeySignals> input::key_d;
utils::ID<input::KeySignals> input::key_space;
utils::ID<input::KeySignals> input::key_z;
utils::ID<input::KeySignals> input::key_f;
utils::ID<input::KeySignals> input::key_n;
utils::ID<input::KeySignals> input::key_j;
utils::ID<input::KeySignals> input::key_up;
utils::ID<input::KeySignals> input::key_down;
utils::ID<input::KeySignals> input::key_left;
utils::ID<input::KeySignals> input::key_right;
utils::ID<input::KeySignals> input::key_o;
utils::ID<input::KeySignals> input::key_l;
utils::ID<input::KeySignals> input::key_lshift;
utils::ID<input::KeySignals> input::key_x;

void input::key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods)
{
  signals::pushEvent(signals::Event<Key, int>((Key)pKey, pAction));
}

void input::char_Callback(GLFWwindow* window, unsigned int pCodepoint)
{
  //printf("char callBack! Char: %c\n", pCodepoint);
}
