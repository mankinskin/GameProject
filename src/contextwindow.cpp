#include "contextwindow.h"
#include "debug.h"
#include "app.h"
#include "gl.h"

int app::monitorCount = 0;
GLFWmonitor** app::allMonitors = nullptr;
app::Monitor app::primaryMonitor;


void app::initMonitors()
{
    puts( "Detecting Monitors..." );
    //find primary monitor for GLFW
    allMonitors = glfwGetMonitors( &monitorCount );
    printf( "%d monitors detected.\n", monitorCount );
    if ( monitorCount > 0 ) {
	primaryMonitor.monitor = allMonitors[0];
    }
    else {
        printf( "Could not detect any monitors.\nAttempting to use primary monitor." );
	primaryMonitor.monitor = glfwGetPrimaryMonitor();
    }
    if ( primaryMonitor.monitor == nullptr ) {
	debug::pushError( "GLFW could not find any monitor!", debug::Error::Fatal );
    }
}

void app::Monitor::init()
{
    //initialize monitor member variables
    vidModes = glfwGetVideoModes( monitor, &videoModeCount );
    currentVideoMode = glfwGetVideoMode( monitor );
    glfwGetMonitorPhysicalSize( monitor, &physical_width, &physical_height );
    pixels_x = currentVideoMode->width;
    pixels_y = currentVideoMode->height;
    dpi_x = ( unsigned int )( ( float )pixels_x / ( 480.0f / 25.4f ) );
    dpi_y = ( unsigned int )( ( float )pixels_y / ( 275.0f / 25.4f ) );
    glfwGetMonitorPos( monitor, &xpos, &ypos );
}

void app::Window::setSize( unsigned int pWidth, unsigned int pHeight )
{
    //set the pixel width and height of the window
    width = pWidth;
    height = pHeight;
}

void app::Window::init()
{
    //sets up GLFW Window with OpenGL context
    if ( window != nullptr ) {
#ifdef _DEBUG
	puts( "\nDestroying current GLFW Window..." );
#endif
	glfwDestroyWindow( window );
    }
#ifdef _DEBUG
    printf( "\nGLFW Window Size:\nX: %i Y: %i\n", width, height );
#endif


    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );
    glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, true );
#ifdef _DEBUG
    //glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
    glfwWindowHint( GLFW_CONTEXT_NO_ERROR, 1 );
#endif
    glfwWindowHint( GLFW_RESIZABLE, 1 );
    glfwWindowHint( GLFW_FOCUSED, 1 );
    glfwWindowHint( GLFW_AUTO_ICONIFY, 1 );
    glfwWindowHint( GLFW_DOUBLEBUFFER, 1 );
    //glfwWindowHint( GLFW_DEPTH_BITS, 24 );
    //glfwWindowHint( GLFW_SAMPLES, 4 );

    window = glfwCreateWindow( width, height, name.c_str(), nullptr, nullptr );

    glfwSetWindowPos( window, ( primaryMonitor.currentVideoMode->width / 2 ) - width / 2, ( primaryMonitor.currentVideoMode->height / 2 ) - height / 2 );
    glfwMakeContextCurrent( window );
    glfwSetWindowUserPointer( window, window );
}
