#pragma once
#include "contextwindow.h"
#include "buttonsignals.h"

namespace input
{
  typedef int KeyData;
  typedef signals::ButtonSignals<KeyData, int> KeySignals;

  struct Key : public utils::ID<KeySignals>
  {
	static utils::Container<KeySignals> all;
	Key(size_t i)
	  : utils::ID<KeySignals>(i, all)
	{}
	Key(utils::ID<KeySignals> id)
	  : utils::ID<KeySignals>(id)
	{}
	Key()
	  : utils::ID<KeySignals>(all)
	{}
  };

  extern Key key_esc;
  extern Key key_c;
  extern Key key_g;
  extern Key key_h;
  extern Key key_i;
  extern Key key_w;
  extern Key key_s;
  extern Key key_a;
  extern Key key_d;
  extern Key key_space;
  extern Key key_z;
  extern Key key_f;
  extern Key key_n;
  extern Key key_j;
  extern Key key_up;
  extern Key key_down;
  extern Key key_left;
  extern Key key_right;
  extern Key key_o;
  extern Key key_l;
  extern Key key_lshift;
  extern Key key_x;

  void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
  void char_Callback(GLFWwindow* window, unsigned int pCodepoint);
}
