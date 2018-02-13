#include "ContextWindow.h"
#include "Debug.h"
#include "app.h"
#include "gl.h"

int app::ContextWindow::monitorCount = 0;
GLFWmonitor** app::ContextWindow::allMonitors = nullptr;
app::ContextWindow::Monitor app::ContextWindow::primaryMonitor;


void app::ContextWindow::initMonitors()
{
    //find primary monitor for GLFW
    allMonitors = glfwGetMonitors(&monitorCount);
    if (monitorCount > 0) {
	primaryMonitor.monitor = allMonitors[0];
    }
    else {
	primaryMonitor.monitor = glfwGetPrimaryMonitor();
    }
    if (primaryMonitor.monitor == nullptr) {
	debug::pushError("GLFW could not find your (primary) monitor!", debug::Error::Fatal);
    }
}

void app::ContextWindow::Monitor::init()
{
    //initialize monitor member variables
    vidModes = glfwGetVideoModes(monitor, &videoModeCount);
    currentVideoMode = glfwGetVideoMode(monitor);
    glfwGetMonitorPhysicalSize(monitor, &physical_width, &physical_height);
    pixels_x = currentVideoMode->width;
    pixels_y = currentVideoMode->height;
    dpi_x = (size_t)((float)pixels_x / (480.0f / 25.4f));
    dpi_y = (size_t)((float)pixels_y / (275.0f / 25.4f));
    glfwGetMonitorPos(monitor, &xpos, &ypos);
}

void app::ContextWindow::Window::setSize(size_t pWidth, size_t pHeight)
{
    //set the pixel width and height of the window
    width = pWidth;
    height = pHeight;
}

void app::ContextWindow::Window::init()
{
    //sets up GLFW Window with OpenGL context
    if (window != nullptr) {
#ifdef _DEBUG
	puts("\nDestroying current GLFW Window...");
#endif
	glfwDestroyWindow(window);
    }
#ifdef _DEBUG
    printf("\nGLFW Window Size:\nX: %i Y: %i\n", width, height);
#endif


    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, 1);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_FOCUSED, 1);
    glfwWindowHint(GLFW_AUTO_ICONIFY, 1);
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    //glfwWindowHint(GLFW_DEPTH_BITS, 24);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwSetWindowPos(window, (primaryMonitor.currentVideoMode->width / 2) - width / 2, (primaryMonitor.currentVideoMode->height / 2) - height / 2);
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, window);
}