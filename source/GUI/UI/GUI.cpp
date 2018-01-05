#include "..\..\Global\stdafx.h"
#include "stdafx.h"
#include "gui.h"
#include "..\..\GlobalGL\glDebug.h"
#include "../../Global/Debug.h"
#include "..\..\BaseGL\VAO.h"
#include "Quad.h"
#include "Line.h"
#include "Colorings.h"
#include "../Text/Text.h"
#include "../../BaseGL/Framebuffer.h"

glm::vec2 gui::pixel_size;

void gui::init()
{
	pixel_size = glm::vec2(2.0f / gl::screenWidth, 2.0f / gl::screenHeight);
	initColors();
	gui::initQuadBuffer();
	gui::initColoringVAOs();
	gui::initQuadBuffer();
	//gui::initLineVAO();
	text::initFonts();
	debug::printErrors();
}

void gui::initColors()
{
	createConstColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "nocolor");
	createConstColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "black");
	createConstColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "white");
	createConstColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "red");
	createConstColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), "green");
	createConstColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), "blue");
	createConstColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), "yellow");
	createConstColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "cyan");
	createConstColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), "magenta");
	createConstColor(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), "purple");
	createConstColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), "grey");
	createConstColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), "darkgrey");
	createConstColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), "lightgrey");
	uploadConstColors();
}
glm::vec2 gui::pixel_round(glm::vec2 pIn) {
	return pixel_size * round(pIn / pixel_size);
}
float gui::pixel_round_x(float pIn) {
	return pixel_size.x * round(pIn / pixel_size.x);
}
float gui::pixel_round_y(float pIn) {
	return pixel_size.y * round(pIn / pixel_size.y);
}