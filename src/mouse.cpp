#include "mouse.h"
#include "app.h"
#include "contextwindow.h"
#include "gl.h"
#include "viewport.h"
#include <algorithm>
#include <array>

using namespace signals;
glm::vec2 input::Cursor::relPos;
glm::uvec2 input::Cursor::absPos;
glm::vec2 input::Cursor::frameDelta;

std::array<bool, 3> mouseKeys;
int scroll = 0;
bool input::Cursor::disabled = false;
bool input::Cursor::hidden = false;
gui::QuadID hovered_quad = gui::QuadID();
gui::QuadID last_hovered_quad = gui::QuadID();
utils::Container<input::MouseKeySignals> input::MouseButton::all = utils::Container<input::MouseKeySignals>();

input::MouseButton input::Mouse::lmb;
input::MouseButton input::Mouse::rmb;
input::MouseButton input::Mouse::mmb;

void input::Cursor::toggleHide()
{
  if (hidden) {
	show();
  } else {
	hide();
  }
}
void input::Cursor::hide()
{
  hidden = true;
  glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}
void input::Cursor::show()
{
  hidden = false;
  glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void input::Cursor::toggle()
{
  if (disabled) {
	enable();
  } else {
	disable();
  }
}
void input::Cursor::disable()
{
  disabled = true;
  glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void input::Cursor::enable()
{
  disabled = false;
  glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void input::updateMouse()
{
  // absolute positions
  double ax = 0.0;
  double ay = 0.0;

  // GLFW window coordinates are from top to bottom all others are bottom to top
  glfwGetCursorPos(app::mainWindow.window, &ax, &ay);
  float rx = (float)ax;
  float ry = (float)ay;
  ax = glm::clamp(ax*gl::Viewport::current->resolution, 0.0, (double)(app::mainWindow.width*gl::Viewport::current->resolution) - 1.0);
  ay = glm::clamp(ay*gl::Viewport::current->resolution, 0.0, (double)(app::mainWindow.height*gl::Viewport::current->resolution) - 1.0);
  if (!Cursor::disabled) {
	rx = (float)ax;// update relative positions too if cursor is not disabled
	ry = (float)ay;
  }
  Cursor::absPos = glm::uvec2((unsigned int)ax, (app::mainWindow.height*gl::Viewport::current->resolution) - (unsigned int)ay - 1);

  rx = ((rx / (float)app::mainWindow.width)*2.0f) - 1.0f;
  ry = 1.0f - (ry / (float)app::mainWindow.height)*2.0f;
  glm::vec2 newRelativeCursorPosition = glm::vec2(rx, ry);
  Cursor::frameDelta = glm::vec2(newRelativeCursorPosition.x - Cursor::relPos.x,
	  newRelativeCursorPosition.y - Cursor::relPos.y);
  Cursor::relPos = newRelativeCursorPosition;
  //printf("%f\t%f\n", Cursor::relPos.x, Cursor::relPos.y);
}

void input::resetMouse()
{
  scroll = 0;
}

void input::getMouseKeyEvents()
{
  static std::array<bool, 3> lastMouseKeys;
  for (unsigned int c = 0; c < 3; ++c) {
	if (mouseKeys[c] != lastMouseKeys[c]) {//mouse key change event
	  pushEvent(Event<MouseKey, bool>(c, mouseKeys[c]));
	  lastMouseKeys[c] = mouseKeys[c];
	}
  }
}

void input::getCursorQuadEvents()
{
  last_hovered_quad = hovered_quad;
  hovered_quad = gui::topQuadAtPosition(Cursor::relPos.x, Cursor::relPos.y);
  //printf("Hovering quad %lu\n", hovered_quad.index);
  if (hovered_quad != last_hovered_quad) {
	if (last_hovered_quad != utils::INVALID_ID) {
	  pushEvent(Event<gui::QuadID, bool>(last_hovered_quad, 0));
	}
	if (hovered_quad != utils::INVALID_ID) {
	  pushEvent(Event<gui::QuadID, bool>(hovered_quad, 1));
	}
  }
}

void input::mouseKey_Callback(GLFWwindow * window, int pKey, int pAction, int pMods)
{
  if (pKey < 3 && pKey >= 0) {
	mouseKeys[pKey] = (bool)pAction;
  }
}

void input::cursorPosition_Callback(GLFWwindow * window, double pX, double pY)
{

}
void input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{

}
void input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{

}
