#pragma once
#include "contextwindow.h"

#include <glew.h>
#include <glm.hpp>

namespace app 
{
    void init();
    void initGLFW();
    void run();
    void quit();
    void mainmenu();

    void setMaxFPS( unsigned int );
    void updateTime();
    void updateTimeFactor();
    void limitFPS();

    extern Window mainWindow;
    extern double timeFactor;
    extern double lastFrameLimitedMS;
    extern double lastFrameMS;
    extern double totalMS;
    extern double minFrameMS;
	extern glm::vec3 node_mov;

    enum State 
    {
        Init,
        MainMenu,
        Running,
        Exit
    };

    extern State state;
}
