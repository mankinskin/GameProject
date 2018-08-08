#pragma once
#include "keys.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include "signal.h"
#include "quad.h"
#include "event.h"

namespace input
{
    typedef int MouseKey;

    namespace Mouse
    {
        extern signals::ButtonSignals<MouseKey> lmb;
        extern signals::ButtonSignals<MouseKey> rmb;
        extern signals::ButtonSignals<MouseKey> mmb;
    };

    extern glm::vec2 relativeCursorPosition;
    extern glm::uvec2 absoluteCursorPosition;
    extern glm::vec2 cursorFrameDelta;
    void updateMouse();
    void resetMouse();
    void toggleCursor();
    void getCursorQuadEvents();
    void getMouseKeyEvents();

    void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
    void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
    void cursorEnter_Callback(GLFWwindow * window, int pEntered);
    void scroll_Callback(GLFWwindow* window, double pX, double pY);
}
