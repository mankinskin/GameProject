#pragma once
#include <glew.h>
#include <glm.hpp>

namespace app
{
  void init();
  void initGLFW();
  void run();
  void quit();
  void mainmenu();

  extern glm::vec3 node_mov;

  enum State
  {
	Init,
	MainMenu,
	Running,
	Exit
  };

  extern State state;
}
