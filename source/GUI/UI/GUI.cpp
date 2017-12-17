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

void gui::init()
{
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
	createConstColor(glm::vec4(0.0, 0.0, 0.0, 0.0), "nocolor");
	createConstColor(glm::vec4(0.0, 0.0, 0.0, 1.0), "black");
	createConstColor(glm::vec4(1.0, 1.0, 1.0, 1.0), "white");
	createConstColor(glm::vec4(1.0, 0.0, 0.0, 1.0), "red");
	createConstColor(glm::vec4(0.0, 1.0, 0.0, 1.0), "green");
	createConstColor(glm::vec4(0.0, 0.0, 1.0, 1.0), "blue");
	createConstColor(glm::vec4(0.5, 0.5, 0.5, 1.0), "grey");
	createConstColor(glm::vec4(0.2, 0.2, 0.2, 1.0), "darkgrey");
	createConstColor(glm::vec4(0.7, 0.7, 0.7, 1.0), "lightgrey");
	uploadConstColors();
}

