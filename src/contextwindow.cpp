#include "contextwindow.h"
#include "app.h"
#include "debug.h"
#include "gl.h"

std::vector<app::Monitor> app::allMonitors;
app::Window app::mainWindow;
const std::string app::Window::DEFAULT_NAME = "OpenGL Window";

void app::startContextwindow()
{
    initMonitors();
    mainWindow = Window( 1600, 850 );
}

app::Monitor::Monitor()
    :ptr( nullptr )
{}

app::Monitor::Monitor( unsigned int pIndex, GLFWmonitor* pMonitor )
    :ptr( pMonitor ), index( pIndex )
{
    puts( "Initalizing Monitor" );
    vidModes = glfwGetVideoModes( ptr, &videoModeCount );
    currentVideoMode = glfwGetVideoMode( ptr );
    glfwGetMonitorPhysicalSize( ptr, &physical_size.x, &physical_size.y );
    pixel_size.x = currentVideoMode->width;
    pixel_size.y = currentVideoMode->height;
    dpi.x = ( unsigned int )( ( float )pixel_size.x / ( 480.0f / 25.4f ) );
    dpi.y = ( unsigned int )( ( float )pixel_size.y / ( 275.0f / 25.4f ) );
    glfwGetMonitorPos( ptr, &pos.x, &pos.y );
    print();
}

void app::Monitor::print()
{
    printf("Monitor %u\nPosX: %u\tPosY: %u\nWidth: %u\tHeight: %u\n", 
            index, pos.x, pos.y, pixel_size.x, pixel_size.y );
}

void app::initMonitors()
{
    puts( "Detecting Monitors..." );
    //find primary monitor for GLFW
    int monitorCount = 0;
    GLFWmonitor** allMonitorPtrs = glfwGetMonitors( &monitorCount );

    if ( monitorCount < 1 ) {
        puts( "Could not detect any monitors.\nAttempting to use primary monitor." );
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if ( primaryMonitor == nullptr ) {
            debug::pushError( "GLFW could not find any monitor!", debug::Error::Fatal );
        }
        else {
            allMonitors.push_back( Monitor( 0, primaryMonitor ) );
        }
    }
    else {
        printf( "%u Monitors detected.\n", monitorCount );
        allMonitors.reserve( monitorCount );
        for( unsigned int m = 0; m < monitorCount; ++m ) {
            allMonitors.push_back( Monitor( m, allMonitorPtrs[m] ) );
        }
    }
}

app::Window::Window( std::string pName, unsigned int pWidth, unsigned int pHeight )
    :name( pName ), width( pWidth ), height( pHeight ), monitor( 0 )
{
    init();
}

app::Window::Window( unsigned int pWidth, unsigned int pHeight )
    :name( DEFAULT_NAME ), width( pWidth ), height( pHeight ), monitor( 0 )
{
    init();
}

app::Window::Window()
    :name(""), width( 0 ), height( 0 ), monitor( 0 )
{
}

void app::Window::init()
{
    if( !allMonitors.size() ) {
        puts( "Can't initialize Window: Monitors are not initialized yet!" );
        return;
    }
    //sets up GLFW Window with OpenGL context
    printf( "Initializing GLFW window %s\n", name.c_str() );

    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );
    glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, true );
    //glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );
    //glfwWindowHint( GLFW_CONTEXT_NO_ERROR, 1 );
    glfwWindowHint( GLFW_RESIZABLE, 1 );
    glfwWindowHint( GLFW_FOCUSED, 1 );
    glfwWindowHint( GLFW_AUTO_ICONIFY, 1 );
    glfwWindowHint( GLFW_DOUBLEBUFFER, 1 );
    //glfwWindowHint( GLFW_DEPTH_BITS, 24 );
    //glfwWindowHint( GLFW_SAMPLES, 4 );

    window = glfwCreateWindow( width, height, name.c_str(), nullptr, nullptr );
    if ( window == nullptr ) {
        puts( "Failed to create GLFW Window!" );  
        return;
    }
    setMonitor();
    center();
    glfwMakeContextCurrent( window );
    glfwSetWindowUserPointer( window, window );
    printf( "GLFW Window Size:\nX: %i Y: %i\n", width, height );
}

void app::Window::setSize( unsigned int pWidth, unsigned int pHeight )
{
    //set the pixel width and height of the window
    width = pWidth;
    height = pHeight;
}

void app::Window::updateMonitor()
{
    GLFWmonitor* mntr = nullptr;
    if ( fullscreen ) {
        width = monitor->pixel_size.x;
        height = monitor->pixel_size.y;
        mntr = monitor->ptr;
    }
    glfwSetWindowMonitor( window, mntr, xpos, xpos, width, height, GLFW_DONT_CARE );
}

void app::Window::setMonitor( app::MonitorID pMonitor )
{
    monitor = pMonitor;
    updateMonitor();
}

void app::Window::setFullscreen()
{
    fullscreen = true;
}

void app::Window::unsetFullscreen()
{
    fullscreen = false;
}

void app::Window::toggleFullscreen()
{
    fullscreen = !fullscreen;
}
void app::Window::updatePos()
{
    glfwSetWindowPos( window, xpos, ypos ); 
}

void app::Window::center()
{
    xpos = ( monitor->currentVideoMode->width / 2 ) - width / 2; 
    ypos = ( monitor->currentVideoMode->height / 2 ) - height / 2;
    updatePos();
}

void app::Window::setPos( unsigned int x, unsigned int y )
{
    xpos = x;
    ypos = y;
    updatePos();
}

void app::Window::destroy()
{
    if ( window != nullptr ) {
        puts( "\nDestroying current GLFW Window..." );
        glfwDestroyWindow( window );
    }
}

