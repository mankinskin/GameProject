#pragma once
#include "keys.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include "signal.h"
#include "buttonsignals.h"
#include "quad.h"
#include "event.h"
#include "utils/id.h"

namespace input
{
  typedef int MouseKey;
  typedef signals::ButtonSignals<MouseKey> MouseKeySignals;

  struct MouseButton : public utils::ID<MouseKeySignals>
  {
	static utils::Container<MouseKeySignals> all;
	MouseButton()
	  : utils::ID<MouseKeySignals>(&all)
	{}
	MouseButton(const size_t i)
	  : utils::ID<MouseKeySignals>(i, &all)
	{}
	MouseButton(const utils::ID<MouseKeySignals>& id)
	  : utils::ID<MouseKeySignals>(std::forward<const utils::ID<MouseKeySignals>&>(id))
	{}
	MouseButton(utils::ID<MouseKeySignals>&& id)
	  : utils::ID<MouseKeySignals>(std::move(id))
	{}
  };
  namespace Mouse
  {
	extern MouseButton lmb;
	extern MouseButton rmb;
	extern MouseButton mmb;
  }

  namespace Cursor
  {
	void toggle();
	void disable();
	void enable();
	void toggleHide();
	void hide();
	void show();
	extern glm::vec2 relPos;
	extern glm::uvec2 absPos;
	extern glm::vec2 frameDelta;
	extern bool disabled;
	extern bool hidden;
  }

  void updateMouse();
  void resetMouse();
  void getCursorQuadEvents();
  void getMouseKeyEvents();

  void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
  void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
  void cursorEnter_Callback(GLFWwindow * window, int pEntered);
  void scroll_Callback(GLFWwindow* window, double pX, double pY);
}
