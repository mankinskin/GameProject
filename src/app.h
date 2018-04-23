#pragma once
#include <glew.h>
#include "contextwindow.h"
#include <glm.hpp>

namespace app {
    enum State {
	Init,
	MainMenu,
	Running,
	Exit
    };

    extern State state;
    void init();
    void initGLFW();
    void run();
    void quit();
    void mainmenu();
    extern Window mainWindow;

    extern double timeFactor;
    extern double lastFrameLimitedMS;
    extern double lastFrameMS;
    extern double totalMS;
    extern double minFrameMS;
    void setMaxFPS( unsigned int pMaxFPS );
    void updateTime();
    void updateTimeFactor();
    void limitFPS();
	extern glm::vec3 node_mov;
}
