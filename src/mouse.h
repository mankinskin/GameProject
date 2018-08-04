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
        const MouseKey button_left = 0;
        const MouseKey button_right = 1;
        const MouseKey button_middle = 2;
        const signals::ButtonSignals<MouseKey> lmb(button_left);
        const signals::ButtonSignals<MouseKey> rmb(button_right);
        const signals::ButtonSignals<MouseKey> mmb(button_middle);
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
