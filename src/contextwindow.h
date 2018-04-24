#pragma once
#include <glew.h>
#include <string>
#include <glfw3.h>
#include <vector>

namespace app 
{
	struct Monitor 
	{
		Monitor();
		Monitor( GLFWmonitor* pMonitor );
		//~Monitor();
	    GLFWmonitor* monitorPtr;
	    int videoModeCount = 0;
	    const GLFWvidmode* vidModes = nullptr;
	    const GLFWvidmode* currentVideoMode = nullptr;
	    int xpos, ypos = 0;
	    unsigned int dpi_x;
	    unsigned int dpi_y;
	    int physical_width;
	    int physical_height;
	    int pixels_x;
	    int pixels_y;
		private:
		void init();
	};

	void initMonitors();
	extern std::vector<Monitor> allMonitors;
	extern GLFWmonitor** allMonitorPtrs;
	extern unsigned int windowMonitor;

	void switchWindowToMonitor( unsigned int pMonitor );

	struct Window 
	{
		const static unsigned int DEFAULT_WIDTH = 1000;
		const static unsigned int DEFAULT_HEIGHT = 700;
		const static std::string DEFAULT_NAME;
		Window( std::string pName, unsigned int pWidth = DEFAULT_WIDTH, unsigned int pHeight = DEFAULT_HEIGHT );
		Window( unsigned int pWidth, unsigned int pHeight );
		Window();

	    void setSize( unsigned int pWidth, unsigned int pHeight );
	    void destroy();

	    GLFWwindow* operator=( const Window& obj ) const 
		{
			return window;
	    }
	    Window& operator=( const Window obj ) 
		{
			name = obj.name;
			width = obj.width;
			height = obj.height;
			window = obj.window;
			return *this;
	    }
		void print()
		{
			printf("Window %s\nWidth: %u\nHeight: %u\nPtr: %p\n", name.c_str(), width, height, window );
		}

	    GLFWwindow* window = nullptr;
	    unsigned int width;
	    unsigned int height;
	    std::string name;
	    bool fullscreen = 0;
	private:
	    void init();
	};
}
