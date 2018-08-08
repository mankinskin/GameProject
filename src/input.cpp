#include "input.h"
#include "mouse.h"
#include "app.h"
#include "debug.h"
#include "camera.h"
#include "gldebug.h"
#include "functor.h"
#include "mesh.h"
#include "light.h"
#include "gui.h"
#include "line.h"
#include "widget.h"
#include "signal.h"
#include "signal.h"
#include "signal.h"
#include "utils/id.h"
#include <tuple>
#include <functional>
#include <algorithm>
#include <array>
#include <glm.hpp>

/*
   button-pipeline
   to set up
   - bind quads as buttons
   - calculateQuadRanges

   every frame
   - fetch Events
   - iterate through buttons
   - get button state and compare it to previous state
   - if different -> generate event
   - check for relevant signals/wheater callbacks should be called
   - iterate through all signals
   - check buttonID-specific range in functor array
   - callButtons
   */

void input::init()
{
    puts("Setting GLFW Callbacks...");
    //initializes GLFW input and defines the keys to track
    glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(app::mainWindow.window, key_Callback);
    glfwSetCharCallback(app::mainWindow.window, char_Callback);
    //glfwSetCursorPosCallback(app::mainWindow.window, cursorPosition_Callback);
    glfwSetCursorEnterCallback(app::mainWindow.window, cursorEnter_Callback);
    glfwSetMouseButtonCallback(app::mainWindow.window, mouseKey_Callback);
    glfwSetScrollCallback(app::mainWindow.window, scroll_Callback);
}

void input::setupControls()
{
    using namespace gui;
    using namespace signals;
    using namespace gates;

    Mouse::lmb = ButtonSignals<MouseKey>(0);
    Mouse::rmb = ButtonSignals<MouseKey>(1);
    Mouse::mmb = ButtonSignals<MouseKey>(2);
    puts("Initializing Key Listeners...");
    key_esc = KeySignals(GLFW_KEY_ESCAPE);
    key_c = KeySignals(GLFW_KEY_C);
    key_g = KeySignals(GLFW_KEY_G);
    key_h = KeySignals(GLFW_KEY_H);
    key_i = KeySignals(GLFW_KEY_I);
    key_w = KeySignals(GLFW_KEY_W);
    key_s = KeySignals(GLFW_KEY_S);
    key_a = KeySignals(GLFW_KEY_A);
    key_d = KeySignals(GLFW_KEY_D);
    key_space = KeySignals(GLFW_KEY_SPACE);
    key_lshift = KeySignals(GLFW_KEY_LEFT_SHIFT);
    key_z = KeySignals(GLFW_KEY_Z);
    key_f = KeySignals(GLFW_KEY_F);
    key_n = KeySignals(GLFW_KEY_N);
    key_j = KeySignals(GLFW_KEY_J);
    key_up = KeySignals(GLFW_KEY_UP);
    key_down = KeySignals(GLFW_KEY_DOWN);
    key_left = KeySignals(GLFW_KEY_LEFT);
    key_right = KeySignals(GLFW_KEY_RIGHT);
    key_o = KeySignals(GLFW_KEY_O);
    key_l = KeySignals(GLFW_KEY_L);
    key_lshift = KeySignals(GLFW_KEY_LEFT_SHIFT);
    key_x = KeySignals(GLFW_KEY_X);

    {
        auto exit_func = functor(app::quit);
        auto toggle_cull_func = functor(mesh::toggleCullFace);
        auto toggle_cursor_func = functor(input::toggleCursor);
        auto toggle_look_func = functor<void(camera::Camera&), camera::Camera&>(camera::toggleLook, camera::main_camera);
        auto toggle_grid_func = functor(glDebug::toggleGrid);
        auto toggle_coord_func = functor(glDebug::toggleCoord);
        auto toggle_info_func = functor(debug::togglePrintInfo);
        auto toggle_normals_func = functor(mesh::toggleNormals);
        auto cycle_cam_modes_func = functor<void(camera::Camera&), camera::Camera&>(camera::cycleModes, camera::main_camera);
        auto higher_cam_speed_func = functor<void(camera::Camera&, float), camera::Camera&, float>(camera::setSpeed, camera::main_camera, 1.0f);
        auto normal_cam_speed_func = functor<void(camera::Camera&, float), camera::Camera&, float>(camera::setSpeed, camera::main_camera, 0.3f);

        link(ifAny(key_c.press(), Mouse::rmb.down(), Mouse::rmb.up()), toggle_look_func);
        link(ifAny(key_c.press(), Mouse::rmb.down(), Mouse::rmb.up()), toggle_cursor_func);
        link(key_esc.press(), exit_func);
        link(key_g.press(), toggle_grid_func);
        link(key_h.press(), toggle_coord_func);
        link(key_n.press(), toggle_normals_func);
        link(key_j.press(), cycle_cam_modes_func);

        //higher_cam_speed_func.add_triggers({ key_lshift.press });
        //normal_cam_speed_func.add_triggers({ key_lshift.release });
    }

    {//camera
        auto forward_func = functor<void(camera::Camera&), camera::Camera&>(camera::forward, camera::main_camera);
        auto backward_func = functor<void(camera::Camera&), camera::Camera&>(camera::back, camera::main_camera);
        auto left_func = functor<void(camera::Camera&), camera::Camera&>(camera::left, camera::main_camera);
        auto right_func = functor<void(camera::Camera&), camera::Camera&>(camera::right, camera::main_camera);
        auto up_func = functor<void(camera::Camera&), camera::Camera&>(camera::up, camera::main_camera);
        auto down_func = functor<void(camera::Camera&), camera::Camera&>(camera::down, camera::main_camera);

        link(key_w.press(), forward_func);
        link(key_w.release(), backward_func);
        link(key_s.press(), backward_func);
        link(key_s.release(), forward_func);
        link(key_a.press(), left_func);
        link(key_a.release(), right_func);
        link(key_d.press(), right_func);
        link(key_d.release(), left_func);
        link(key_space.press(), up_func);
        link(key_space.release(), down_func);
        link(key_lshift.press(), down_func);
        link(key_lshift.release(), up_func);
    }
}

void input::fetchGLFWEvents()
{
    //updates the states of the mouse buttons, mouse wheel and all tracked keys
    glfwPollEvents();
}


void input::end()
{
    resetMouse();
    if (glfwWindowShouldClose(app::mainWindow.window)) {
        app::state = app::State::Exit;
    }
}
