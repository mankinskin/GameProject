#include "app.h"
#include "debug.h"
#include "input.h"
#include "keys.h"
#include "functor.h"
#include "mouse.h"
#include "event.h"
#include "signal.h"
//#ifdef WINDOWS
#include <ncurses.h>
//#else
#include <curses.h>
//#endif
#include <thread>
#include <chrono>
#include "gl.h"
#include "text.h"
#include "camera.h"
#include "gldebug.h"
#include "font_loader.h"
#include "gui.h"
#include "model.h"
#include "mesh.h"
#include "lights.h"
#include "colorings.h"
#include "framebuffer.h"
#include "quad.h"
#include <functional>
#include <algorithm>
#include "entities.h"
#include "contextwindow.h"
#include "physics.h"
#include "voxelization.h"
#include "line.h"
app::State app::state = app::State::Init;
app::ContextWindow::Window app::mainWindow = app::ContextWindow::Window();
double app::timeFactor = 1.0;
double app::lastFrameMS = 0;
double app::lastFrameLimitedMS = 0;
double app::totalMS = 0;
double app::minFrameMS = 8.0;


glm::vec3 app::node_mov = glm::vec3();

void app::init()
{
	state = Running;
	setMaxFPS(10000);
	initGLFW();
	//Windows and gl Context
	ContextWindow::initMonitors();
	ContextWindow::primaryMonitor.init();
	mainWindow.setSize(1600, 850);
	mainWindow.init();
	//Input listeners
	Input::init();
	gl::init();

	//gui::text::initStyleBuffer();
	Input::setupControls();
	
	debug::printErrors();

	while (app::state != app::State::Exit) {
		app::gameloop();
	}
}


void app::gameloop()
{	
	GLuint gui_clear_index[4] = { 0, 0, 0, 0 };
	GLfloat g_clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat g_clear_depth = 1.0f;
	
	while (state == app::Running) {
		fetchInput();
		//entities::translate(0, node_mov * 0.01f);
		//node_mov = glm::vec3();

		camera::main_camera.look(Input::cursorFrameDelta);
		camera::main_camera.update();

		gl::updateGeneralUniformBuffer();
		lighting::updateLightIndexRangeBuffer();
		lighting::updateLightDataBuffer();
		entities::updateEntityMatrices();
		entities::updateEntityBuffers();
		mesh::updateMeshBuffers();
		gui::updateLineBuffers();
		gui::updateQuadBuffer();
		gui::updateColorings();
		//gui::text::updateCharStorage();

		//reset g_buffer
		//glClearNamedFramebufferfv(texture::gBuffer, GL_COLOR, 0, g_clear_color);
		//glClearNamedFramebufferfv(texture::gBuffer, GL_COLOR, 1, g_clear_color);
		//glClearNamedFramebufferfv(texture::gBuffer, GL_COLOR, 2, g_clear_color);
		//glClearNamedFramebufferfv(texture::gBuffer, GL_COLOR, 3, g_clear_color);
		//glClearNamedFramebufferfv(texture::gBuffer, GL_COLOR, 4, g_clear_color);
		//glClearNamedFramebufferfv(texture::gBuffer, GL_DEPTH, 0, &g_clear_depth);
		//clear screen buffer
		voxelization::clearVolumeTexture();
		glClearNamedFramebufferfv(0, GL_COLOR, 0, g_clear_color);
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, &g_clear_depth);
		////reset guiFBO
		glClearNamedFramebufferuiv(texture::guiFBO, GL_COLOR, 0, gui_clear_index);
		glClearNamedFramebufferfv(texture::guiFBO, GL_DEPTH, 0, &g_clear_depth);

		glBindFramebuffer(GL_FRAMEBUFFER, texture::guiFBO);
		gui::rasterQuadIndices();
		gui::readQuadIndexBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		voxelization::voxelizeMeshes();
		gui::renderLines();
		//glBindFramebuffer(GL_FRAMEBUFFER, texture::gBuffer);
		//mesh::renderMeshes();

		//glBindFramebuffer(GL_READ_FRAMEBUFFER, texture::gBuffer);
		
		//glBlitFramebuffer(0, 0, gl::screenWidth, gl::screenHeight, 0, 0, gl::screenWidth, gl::screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		lighting::renderLights();

		//mesh::renderMeshNormals();
		
		gui::renderColorings();
		//gui::text::renderGlyphs();
		
		glfwSwapBuffers(mainWindow.window);

		debug::printErrors();
		updateTime();
		updateTimeFactor();
		limitFPS();
		debug::printInfo();
	}
	gui::text::clearCharStorage();


	gui::clearQuads();
	functors::clearFunctors();
	signals::clearSignals();
}

void app::fetchInput()
{
	Input::updateMouse();
	Input::fetchGLFWEvents();

	Input::getMouseEvents();
	events::checkEvents();
	signals::checkSignals();
	functors::callFunctors();

	events::resetEvents();
	signals::resetSignals();
	Input::end();

}

void app::initGLFW()
{
	std::puts("Initializing GLFW.../n");
	size_t glfw = glfwInit();
	if (glfw != GLFW_TRUE) {
		debug::pushError(("/napp::init() - Unable to initialize GLFW (glfwInit() return code: %i)/n" + glfw), debug::Error::Severity::Fatal);
		//while (!getch()) {}
		exit(glfw);
	}
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
	if (lastFrameMS < minFrameMS) {
		lastFrameLimitedMS = minFrameMS;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(minFrameMS - lastFrameMS)));
	}
}

void app::updateTimeFactor() {
	timeFactor = 1.0f;
}

void app::setMaxFPS(size_t pMaxFPS)
{
	minFrameMS = (size_t)(1000.0f / (float)pMaxFPS);
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
