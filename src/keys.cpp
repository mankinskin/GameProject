#include "keys.h"
#include "signal.h"

input::Key input::key_esc;
input::Key input::key_c;
input::Key input::key_g;
input::Key input::key_h;
input::Key input::key_i;
input::Key input::key_w;
input::Key input::key_s;
input::Key input::key_a;
input::Key input::key_d;
input::Key input::key_space;
input::Key input::key_z;
input::Key input::key_f;
input::Key input::key_n;
input::Key input::key_j;
input::Key input::key_up;
input::Key input::key_down;
input::Key input::key_left;
input::Key input::key_right;
input::Key input::key_o;
input::Key input::key_l;
input::Key input::key_lshift;
input::Key input::key_x;

utils::Container<input::KeySignals> input::Key::all = utils::Container<input::KeySignals>();

void input::key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods)
{
  signals::pushEvent(signals::Event<KeyData, int>((KeyData)pKey, pAction));
}

void input::char_Callback(GLFWwindow* window, unsigned int pCodepoint)
{
  //printf("char callBack! Char: %c\n", pCodepoint);
}
