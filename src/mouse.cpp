#include "mouse.h"
#include "app.h"
#include "contextwindow.h"
#include <algorithm>
#include "event.h"
#include <array>
#include "quad.h"
#include "gl.h"

using namespace events;
glm::vec2 app::Input::relativeCursorPosition;
glm::uvec2 app::Input::absoluteCursorPosition;
glm::vec2 app::Input::cursorFrameDelta;

std::array<app::Input::KeyCondition, 3> mouseKeys;
int scroll = 0;
int disableCursor = 0;
size_t hovered_quad = 0;
size_t last_hovered_quad = 0;

void app::Input::updateMouse() {
	//update cursor pos
	double ax = 0.0;//absolute positions
	double ay = 0.0;
	
	//GLFW window coordinates are from top to bottom all others are bottom to top
	glfwGetCursorPos(app::mainWindow.window, &ax, &ay);
	float rx = (float)ax;
	float ry = (float)ay;
	ax = glm::clamp(ax*gl::resolution, 0.0, (double)(mainWindow.width*gl::resolution) - 1.0);
	ay = glm::clamp(ay*gl::resolution, 0.0, (double)(mainWindow.height*gl::resolution) - 1.0);
	if (!disableCursor) {
		rx = (float)ax;//clamp relative positions too if cursor is not disabled
		ry = (float)ay;
	}
	absoluteCursorPosition = glm::uvec2((unsigned int)ax, (mainWindow.height*gl::resolution) - (unsigned int)ay - 1);

	rx = ((rx / (float)app::mainWindow.width)*2.0f) - 1.0f;
	ry = 1.0f - (ry / (float)app::mainWindow.height)*2.0f;
	glm::vec2 newRelativeCursorPosition = glm::vec2(rx, ry);
	cursorFrameDelta = glm::vec2((newRelativeCursorPosition.x - relativeCursorPosition.x),
		(newRelativeCursorPosition.y - relativeCursorPosition.y));
	relativeCursorPosition = newRelativeCursorPosition;

	//printf("%f/n%f/n/n/n", pX, pY);
	//printf("%i/n%i/n/n/n", absoluteCursorPosition.x, absoluteCursorPosition.y);
	
}

void app::Input::resetMouse()
{
	scroll = 0;
}

void app::Input::toggleCursor()
{
	disableCursor = !disableCursor;
	if (disableCursor) {
		glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void app::Input::getMouseKeyEvents(){
	static std::array<KeyCondition, 3> lastMouseKeys;

	for (unsigned int c = 0; c < 3; ++c) {
		if (mouseKeys[c] != lastMouseKeys[c]) {//mouse key change event
			pushEvent(MouseKeyEvent(c, mouseKeys[c]));
		}
	}
	
	lastMouseKeys = mouseKeys;
}
void app::Input::getCursorQuadEvents(){
	last_hovered_quad = hovered_quad;
	hovered_quad = gui::readQuadIndexMap(absoluteCursorPosition.x, absoluteCursorPosition.y);
	float quad_depth = gui::readQuadDepthMap(absoluteCursorPosition.x, absoluteCursorPosition.y);
	//printf("%i/t%f/n", hovered_quad, quad_depth);

	if(last_hovered_quad != hovered_quad){
		//quad change
		if (last_hovered_quad) {
			//button leave event
			pushEvent(QuadEvent(last_hovered_quad, 0));
		}
		if (hovered_quad) {
			//button enter event
			pushEvent(QuadEvent(hovered_quad, 1));
		}
	}
}

void app::Input::mouseKey_Callback(GLFWwindow * window, int pKey, int pAction, int pMods)
{
	if (pKey < 3 && pKey >= 0) {
		mouseKeys[pKey] = KeyCondition(pAction);
	}
}

void app::Input::cursorPosition_Callback(GLFWwindow * window, double pX, double pY)
{

}
void app::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{

}
void app::Input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{
	//Entered = 1 if entered, on exit = 0
}
