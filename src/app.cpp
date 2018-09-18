#include "app.h"
#include "debug.h"
#include "gl.h"
#include "gldebug.h"
#include "contextwindow.h"
#include "sequencer.h"

#ifdef WINDOWS
#   include <ncurses.h>
#else
#   include <curses.h>
#endif
#include "simtime.h"

app::State app::state = app::State::Init;

glm::vec3 app::node_mov = glm::vec3();

void app::init()
{
  puts("random message");
  state = Running;
  simtime::setFpsCap(120);
  // Windows and gl Context
  initGLFW();
  startContextwindow();
  gl::init();

  sequencer::initialize();
  //gui::text::initStyleBuffer();

  debug::printErrors();

  while (app::state != app::State::Exit) {
	sequencer::gameloop();
  }
}

void app::initGLFW()
{
  std::puts("Initializing GLFW...");
  unsigned int glfw = glfwInit();
  if (glfw != GLFW_TRUE) {
	debug::fatal("app::init() - Unable to initialize GLFW (glfwInit() return code: %i)\n" + std::to_string(glfw));
	while (!getch()) {}
	exit(glfw);
  }
  std::puts("Initialized GLFW successfully.");
}

void app::run()
{
  state = app::State::Running;
}

void app::quit()
{
  state = app::State::Exit;
}

void app::mainmenu()
{
  state = app::State::MainMenu;
}

