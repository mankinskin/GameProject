#include "contextwindow.h"
#include "app.h"
#include "debug.h"
#include "gl.h"

GLFWmonitor** app::allMonitorPtrs = nullptr;
std::vector<app::Monitor> app::allMonitors;
app::Monitor* app::windowMonitor = nullptr;
const std::string app::Window::DEFAULT_NAME = "OpenGL Window";

void app::initMonitors()
{
    puts( "Detecting Monitors..." );
    //find primary monitor for GLFW
    int monitorCount = 0;
    allMonitorPtrs = glfwGetMonitors( &monitorCount );
    printf( "%d monitors detected.\n", monitorCount );

    if ( monitorCount < 1 ) {
        puts( "Could not detect any monitors.\nAttempting to use primary monitor." );
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if ( primaryMonitor == nullptr ) {
            debug::pushError( "GLFW could not find any monitor!", debug::Error::Fatal );
        }
        else {
            allMonitors.push_back( Monitor( primaryMonitor ) );
        }
    }
    else {
        allMonitors.reserve( monitorCount );
        for( unsigned int m = 0; m < monitorCount; ++m ) {
            allMonitors.push_back( Monitor( allMonitorPtrs[m] ) );	
        }
        windowMonitor = &allMonitors[0];
    }
}

app::Monitor::Monitor()
    :monitorPtr( nullptr )
{
}

app::Monitor::Monitor( GLFWmonitor* pMonitor )
    :monitorPtr( pMonitor )
{
    puts( "Initalizing Monitor" );
    vidModes = glfwGetVideoModes( monitorPtr, &videoModeCount );
    currentVideoMode = glfwGetVideoMode( monitorPtr );
    glfwGetMonitorPhysicalSize( monitorPtr, &physical_size.x, &physical_size.y );
    pixel_size.x = currentVideoMode->width;
    pixel_size.y = currentVideoMode->height;
    dpi.x = ( unsigned int )( ( float )pixel_size.x / ( 480.0f / 25.4f ) );
    dpi.y = ( unsigned int )( ( float )pixel_size.y / ( 275.0f / 25.4f ) );
    glfwGetMonitorPos( monitorPtr, &pos.x, &pos.y );
}

void app::Window::setSize( unsigned int pWidth, unsigned int pHeight )
{
    //set the pixel width and height of the window
    width = pWidth;
    height = pHeight;
}

void app::switchWindowToMonitor( unsigned int pMonitor )
{
    if ( pMonitor < allMonitors.size() ) 
        windowMonitor = &allMonitors[ pMonitor ];
}

void app::Window::init()
{
    if( !allMonitors.size() ) {
        puts( "Can't initialize Window: Monitors are not initialized yet!" );
        return;
    }
    //sets up GLFW Window with OpenGL context
    printf( "Initializing GLFW window %s\n", name.c_str() );
    printf( "GLFW Window Size:\nX: %i Y: %i\n", width, height );

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
        puts( "Failed to create GLFW Window! window was nullptr!" );  
        return;
    }
    puts( "Successfully initialized Window" );
    glfwSetWindowPos( window, 
            ( windowMonitor->currentVideoMode->width / 2 ) - width / 2, 
            ( windowMonitor->currentVideoMode->height / 2 ) - height / 2 );
    glfwMakeContextCurrent( window );
    glfwSetWindowUserPointer( window, window );
    if ( window == nullptr ) {
        puts( "Failed to create GLFW Window! window was nullptr!" );  
        return;
    }
}

app::Window::Window( std::string pName, unsigned int pWidth, unsigned int pHeight )
    :name( pName ), width( pWidth ), height( pHeight )
{
    init();
}

app::Window::Window( unsigned int pWidth, unsigned int pHeight )
    :name( DEFAULT_NAME ), width( pWidth ), height( pHeight )
{
    init();
}

app::Window::Window()
    :name(""), width( 0 ), height( 0 )
{
}

void app::Window::destroy()
{
    if ( window != nullptr ) {
        puts( "\nDestroying current GLFW Window..." );
        glfwDestroyWindow( window );
    }
}

