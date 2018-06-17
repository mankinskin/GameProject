#pragma once
#include "utils/id.h"
#include <glew.h>
#include <glm.hpp>
#include <string>
#include <glfw3.h>
#include <vector>

namespace app 
{
    struct Monitor 
    {
        Monitor();
        Monitor( unsigned int pIndex, GLFWmonitor* pMonitor );
        //~Monitor();
        GLFWmonitor* ptr;
        int videoModeCount = 0;
        const GLFWvidmode* vidModes = nullptr;
        const GLFWvidmode* currentVideoMode = nullptr;
        glm::uvec2 dpi;
        int physical_width = 0;
        int physical_height = 0;
        glm::ivec2 pos;
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int index = 0;
        void print();
    };

    using MonitorID = utils::ID<Monitor>;

    struct Window 
    {
        const static unsigned int DEFAULT_WIDTH = 1000;
        const static unsigned int DEFAULT_HEIGHT = 700;
        const static std::string DEFAULT_NAME;
        Window( std::string pName, unsigned int pWidth = DEFAULT_WIDTH, unsigned int pHeight = DEFAULT_HEIGHT );
        Window( unsigned int pWidth, unsigned int pHeight );
        Window();

        void setSize( unsigned int pWidth, unsigned int pHeight );
        void setFullscreen();
        void unsetFullscreen();
        void toggleFullscreen();
        void setMonitor( MonitorID pMonitor = MonitorID( 0 ) );
        void destroy();
        void center();
        void setPos( unsigned int x, unsigned int y );
        void updateMonitor();
        void updatePos();

        GLFWwindow* operator=( const Window& obj ) const 
        {
            return window;
        }

        Window& operator=( const Window obj ) 
        {
            name = obj.name;
            xpos = obj.xpos;
            ypos = obj.ypos;
            width = obj.width;
            height = obj.height;
            window = obj.window;
            return *this;
        }

        void print()
        {
            printf("Window %s\nWidth: %u\nHeight: %u\n", name.c_str(), width, height );
        }

        GLFWwindow* window = nullptr;
        MonitorID monitor;        
        unsigned int xpos = 0;
        unsigned int ypos = 0;
        unsigned int width;
        unsigned int height;
        std::string name;
        bool fullscreen = false;

        private:
        void init();
    };

    void initMonitors();
    void startContextwindow();
    extern Window mainWindow;
}
