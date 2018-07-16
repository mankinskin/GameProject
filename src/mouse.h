#pragma once
#include "keys.h"
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include "quad.h"

namespace input
{
    struct MouseKeyEvent    // signals thrown when a mouse key changes
    {
        MouseKeyEvent()
            :key(-1)
        {}
        MouseKeyEvent(int pKey, int pAction)
            :key(pKey), change(pAction)
        {}
        int key;
        int change;
    };
    inline bool operator==(MouseKeyEvent const & l, MouseKeyEvent const& r)
    {
        return l.key == r.key && l.change == r.change;
    }

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
