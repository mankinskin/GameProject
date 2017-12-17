#include "stdafx.h"
#include "app.h"
#include "Debug.h"
#include "..\Input\Input.h"
#include "..\Input\Keys.h"
#include "..\Input\Functor.h"
#include "..\Input\Mouse.h"
#include "..\Input\Event.h"
#include <conio.h>
#include <thread>
#include <chrono>
#include "../GlobalGL/gl.h"
#include "../gui\text\text.h"
#include "../BaseGL/camera.h"
#include "../GlobalGL/glDebug.h"
#include "../gui\text\Font_Loader.h"
#include "../gui\UI\gui.h"
#include "../Model\Model.h"
#include "../Mesh\Mesh.h"
#include "../lighting\Lights.h"
#include "../gui/UI/Colorings.h"
#include "../BaseGL/Framebuffer.h"
#include "../gui\UI\Quad.h"
#include "../gui\UI\Element.h"
#include "../gui\UI\Widget.h"
#include <functional>
#include <algorithm>
#include "../Model/Node.h"
#include "../BaseGL/ContextWindow.h"
#include "../physics/physics.h"

app::State app::state = app::State::Init;
app::ContextWindow::Window app::mainWindow = app::ContextWindow::Window();
double app::timeFactor = 1.0;
double app::lastFrameMS = 0;
double app::lastFrameLimitedMS = 0;
double app::totalMS = 0;
double app::targetFrameMS = 16.0;


glm::vec3 app::node_mov = glm::vec3();

void app::init()
{
	state = MainMenu;
	initGLFW();
	//Windows and gl Context
	ContextWindow::initMonitors();
	ContextWindow::primaryMonitor.init();
	mainWindow.setSize(1600, 850);
	mainWindow.init();
	//Input listeners
	Input::init();
	gl::init();
	gui::text::initStyleBuffer();

	debug::printErrors();
}

void app::initGLFW()
{
	std::puts("Initializing GLFW...\n");
	size_t glfw = glfwInit();
	if (glfw != GLFW_TRUE) {
		debug::pushError(("\napp::init() - Unable to initialize GLFW (glfwInit() return code: %i)\n" + glfw), debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glfw);
	}
}


void app::mainMenuLoop()
{
	lighting::createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 10.0f));
	lighting::createLight(glm::vec4(0.0f, 3.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.4f, 0.5f, 10.0f));
	Input::setupControls();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gl::screenWidth, gl::screenHeight);
	debug::printErrors();
	while (state == app::MainMenu) {

		node::translate(0, node_mov * 0.01f);
		camera::main_camera.look(Input::cursorFrameDelta);
		node_mov = glm::vec3();

		camera::main_camera.update();
		gl::updateGeneralUniformBuffer();
		lighting::updateLightIndexRangeBuffer();
		lighting::updateLightDataBuffer();
		node::updateNodeMatrices();
		node::updateNodeBuffers();
		mesh::updateMeshBuffers();

		glBindFramebuffer(GL_FRAMEBUFFER, texture::gBuffer);
		glViewport(0, 0, size_t(gl::screenWidth*gl::resolution), size_t(gl::screenHeight*gl::resolution));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mesh::renderMeshes();
		mesh::updateMeshBuffers();
		gui::updateQuadBuffer();
		gui::updateColorings();
		gui::text::updateCharStorage();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lighting::renderLights();
		gui::renderColorings();
		gui::text::renderGlyphs();
		glDebug::drawGrid();
		mesh::renderMeshNormals();
		glBindFramebuffer(GL_FRAMEBUFFER, texture::guiFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui::rasterQuadIndices();
		gui::readQuadIndexBuffer();
		fetchInput();
		glfwSwapBuffers(mainWindow.window);
		debug::printErrors();
		updateTime();
		updateTimeFactor();
		//limitFPS();

		debug::printInfo();
	}
	gui::text::clearCharStorage();

	gui::clearQuads();
	Input::clearFunctors();
	Input::clearSignals();
}

void app::fetchInput()
{

	Input::updateMouse();
	Input::fetchGLFWEvents();

	Input::getMouseEvents();
	Input::checkEvents();
	Input::callFunctors();
	Input::resetSignals();
	Input::end();
}

//--Global Time--
void app::updateTime()
{
	double thisFrameMS = (glfwGetTime() * 1000.0);
	lastFrameMS = thisFrameMS - totalMS;
	totalMS = thisFrameMS;
}

void app::limitFPS()
{
	lastFrameLimitedMS = lastFrameMS;
	if (lastFrameMS < targetFrameMS) {
		lastFrameLimitedMS = targetFrameMS;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(targetFrameMS - lastFrameMS)));
	}
}

void app::updateTimeFactor() {
	timeFactor = lastFrameMS / targetFrameMS;
}

void app::setTargetFPS(size_t pTargetFPS)
{
	targetFrameMS = (size_t)(1000.0f / (float)pTargetFPS);
}

void app::run() {
	state = app::State::Running;
}

void app::quit() {
	state = app::State::Exit;
}

void app::mainmenu()
{
	state = app::State::MainMenu;
}
