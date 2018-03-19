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
#include "quadcolors.h"
#include "framebuffer.h"
#include "quad.h"
#include <functional>
#include <algorithm>
#include "entities.h"
#include "contextwindow.h"
#include "physics.h"
#include "voxelization.h"
#include "line.h"
#include "sequencer.h"

app::State app::state = app::State::Init;
app::Window app::mainWindow = app::Window();
double app::timeFactor = 1.0;
double app::lastFrameMS = 0;
double app::lastFrameLimitedMS = 0;
double app::totalMS = 0;
double app::minFrameMS = 8.0;


glm::vec3 app::node_mov = glm::vec3();

void app::init()
{
	puts( "Hello" );
	state = Running;
	setMaxFPS( 1000 );
	initGLFW();
	//Windows and gl Context
	initMonitors();
	primaryMonitor.init();
	mainWindow.setSize( 1600, 850 );
	mainWindow.init();

	gl::init();
    sequencer::includeShaders();
    sequencer::initializeVAOs();
    sequencer::buildShaders();
    sequencer::initModules();
	//gui::text::initStyleBuffer();

	debug::printErrors();

	while ( app::state != app::State::Exit ) {
		sequencer::gameloop();
	}
}




void app::initGLFW()
{
	std::puts( "Initializing GLFW...\n" );
	unsigned int glfw = glfwInit();
	if ( glfw != GLFW_TRUE ) {
		debug::pushError( ( "\napp::init() - Unable to initialize GLFW ( glfwInit() return code: %i )\n" + glfw ), debug::Error::Severity::Fatal );
		//while ( !getch() ) {}
		exit( glfw );
	}
}
//--Global Time--
void app::updateTime()
{
	double thisFrameMS = ( glfwGetTime() * 1000.0 );
	lastFrameMS = thisFrameMS - totalMS;
	totalMS = thisFrameMS;
}

void app::limitFPS()
{
	lastFrameLimitedMS = lastFrameMS;
	if ( lastFrameMS < minFrameMS ) {
		lastFrameLimitedMS = minFrameMS;
		std::this_thread::sleep_for( std::chrono::milliseconds( ( int )( minFrameMS - lastFrameMS ) ) );
	}
}

void app::updateTimeFactor() {
	timeFactor = 1.0f;
}

void app::setMaxFPS( unsigned int pMaxFPS )
{
	minFrameMS = ( unsigned int )( 1000.0f / ( float )pMaxFPS );
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
