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
#include <chrono>
#include <thread>

app::State app::state = app::State::Init;
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
    setMaxFPS( 50 );
    // Windows and gl Context
    initGLFW();
    startContextwindow();
    gl::init();

    sequencer::initialize();
    //gui::text::initStyleBuffer();

    debug::printErrors();

    while ( app::state != app::State::Exit ) {
        sequencer::gameloop();
    }
}

void app::initGLFW()
{
    std::puts( "Initializing GLFW..." );
    unsigned int glfw = glfwInit();
    if ( glfw != GLFW_TRUE ) {
        debug::pushError( ( "app::init() - Unable to initialize GLFW ( glfwInit() return code: %i )\n" + glfw ), debug::Error::Severity::Fatal );
        //while ( !getch() ) {}
        exit( glfw );
    }
    std::puts( "Initialized GLFW successfully." );
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

void app::updateTimeFactor() 
{
    timeFactor = 1.0f;
}

void app::setMaxFPS( unsigned int pMaxFPS )
{
    minFrameMS = ( unsigned int )( 1000.0f / ( float )pMaxFPS );
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

